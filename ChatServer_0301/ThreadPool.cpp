////
//// Created by zy on 2022/1/14.
////
//#include "ThreadPool.h"
//
//
//
//    ThreadPool::ThreadPool(int numWorkers):m_isClose(false) {
//        numWorkers = numWorkers <= 0 ? 1:numWorkers;
//
//        for(int i=0; i<numWorkers; ++i){
//            m_threads.emplace_back([this]{  // lambda 函数
//                while(1){
//                    JobFunction job;
//                    {
//                        std::unique_lock<std::mutex> lck(m_mtx);
//                        while(!m_isClose && m_jobs.empty()){
//                            m_cond.wait(lck);
//                        }
//                        if(m_jobs.empty() && m_isClose){
//                            //printf("[ThreadPool::TheadPool] thread_id = %lu return\n", std::this_thread::get_id());
//                            return;
//                        }
//                        job = m_jobs.front();
//                        m_jobs.pop();
//                    }
//                    if(job){
//                        //printf("[ThreadPool::ThreadPool] thread_id = %lu get a job\n", std::this_thread::get_id());
//                        job();
//                        //printf("[ThreadPool::ThreadPool] thread_id = %lu job finish\n", std::this_thread::get_id());
//                    }
//                }
//            });
//        }
//    }
//    ThreadPool::~ThreadPool() {
//        printf("begin des\n");
//        {
//            std::unique_lock<std::mutex> lck(m_mtx);
//            m_isClose = true;
//        }
//        m_cond.notify_all();
//        for (auto &thread : m_threads)
//        {
//            thread.join();
//        }
//        printf("[ThreadPool::~ThreadPool] threadpool is remove\n");
//    }
//
//    void ThreadPool::pushJob(const JobFunction &job)
//    {
//        {
//            std::unique_lock<std::mutex> lck(m_mtx);
//            //printf("[ThreadPool::pushJob] push new job\n");
//            m_jobs.push(job);
//        }
//
//        m_cond.notify_one();
//    }



//
// Created by zhou on 2022/3/8.
//

#include "ThreadPool.h"

ThreadPool::ThreadPool(int numCoreWorkers){
    m_coreNumWorkers = numCoreWorkers;
    m_MaxNumWorkers = 8;
    m_sleepTime = std::chrono::seconds(60);


    thread_waiting_num = 0;
    thread_existing_num = 0;
    thread_id.store(0);
    is_shutdown.store(false);
    is_shutdown_now.store(false);
    run();
}
ThreadPool::ThreadPool(int numCoreWorkers, int numMaxWorkers, std::chrono::seconds sleepTime) {
    m_coreNumWorkers = numCoreWorkers;
    m_MaxNumWorkers = numMaxWorkers;
    m_sleepTime = sleepTime;


    thread_waiting_num = 0;
    thread_existing_num = 0;
    thread_id.store(0);
    is_shutdown.store(false);
    is_shutdown_now.store(false);
}

// 启动线程池
void ThreadPool::run() {
    while(getTotalThread() < m_coreNumWorkers)
        AddThread();
}

void ThreadPool::runAsSingle() {
    AddThread();
}

ThreadPool::~ThreadPool() {
    printf("begin destructor\n");
    shutDown();

}

void ThreadPool::pushJob(const JobFunction &job)
{
    {
        std::unique_lock<std::mutex> lck(m_mtx);
        printf("[ThreadPool::pushJob] push new job\n");
        std::cout << "thread_existing_num=" << thread_existing_num << ", thread_waiting_num="<< thread_waiting_num << std::endl;
        //        m_jobs.push(job);
        m_jobs.emplace(job);
    }
    if(getWaitingThread() == 0 && getTotalThread() < m_MaxNumWorkers){
        // 所有线程都已经被占满, 此时应该添加新的线程
        AddThread();
    }

    m_cond.notify_one();    // 如果没有得到回应，这里会发生阻塞
}
void ThreadPool::AddThread(){
    if(thread_existing_num < m_coreNumWorkers){
        AddThread(thread_id++, ThreadFlag::e_core);
    }else{
        AddThread(thread_id++, ThreadFlag::e_cache);
    }
}
void ThreadPool::AddThread(int id, ThreadFlag thread_flag) {
//    std::cout << "ThreadPool::AddThread 新线程：" << id << "，Flag："  << static_cast<int>(thread_flag) << std::endl;
    ++this->thread_existing_num;
    ThreadWrapperPtr thread_ptr = std::make_shared<ThreadWrapper>();
    thread_ptr->id_.store(id);
    thread_ptr->flag_.store(thread_flag);
    auto func = [this, thread_ptr](){

        for(;;){
            std::function<void()> task = nullptr;
            {
                std::unique_lock<std::mutex> lck(this->m_mtx);
                if(thread_ptr->state_ == ThreadState::e_stop){
                    break;
                }
//                std::cout << "thread_id = " << thread_ptr->id_.load() << ", waiting for job\n";
                thread_ptr->state_ = ThreadState::e_waiting;
                ++this->thread_waiting_num;
                bool timeout = false;
                if(thread_ptr->flag_ == ThreadFlag::e_core){
//                    this->m_cond.wait(lck);
                    this->m_cond.wait(lck, [this, thread_ptr](){
                        return (this->is_shutdown || this->is_shutdown_now || (!this->m_jobs.empty())
                                || thread_ptr->state_ == ThreadState::e_stop);
                    });
                }else{
                    this->m_cond.wait_for(lck, m_sleepTime, [this, thread_ptr](){
                        return (this->is_shutdown || this->is_shutdown_now || !this->m_jobs.empty()
                        || thread_ptr->state_ == ThreadState::e_stop);
                    });
                    timeout = !(this->is_shutdown || this->is_shutdown_now || !this->m_jobs.empty()
                            || thread_ptr->state_ == ThreadState::e_stop);
                }
                // 如果是休眠时间太长，则将该线程进行回收
                if(timeout){
                    thread_ptr->state_.store(ThreadState::e_stop);
                }
                if(thread_ptr->state_ == ThreadState::e_stop){
                    break;
                }
                if(this->is_shutdown && this->m_jobs.empty()){
//                    std::cout << "thread_id = " << thread_ptr->id_.load() << " shutdown\n";
                    break;
                }
                if(this->is_shutdown_now){
//                    std::cout << "thread_id = " << thread_ptr->id_.load() << " shutdown now\n";
                    break;
                }
                thread_ptr->state_ = ThreadState::e_running;
                task = std::move(this->m_jobs.front());
                this->m_jobs.pop();
                --this->thread_waiting_num;
                std::cout << "thread_id = " << thread_ptr->id_.load() << ", running for job\n";
            }
            if(task != nullptr){
                task();
                std::cout << "thread_id = " << thread_ptr->id_.load() << ", finish job\n";
            }
        }
        --this->thread_existing_num;
        std::cout << "thread id " << thread_ptr->id_.load() << " running end" << std::endl;
        {
            std::unique_lock<std::mutex> lck(m_mtx_threads);
            m_threads.erase(thread_ptr->id_);
        }
//                std::cout << "m_threads size = " << m_threads.size() << std::endl;
    };
    thread_ptr->ptr_ = std::make_shared<std::thread>(std::move(func));
    if(thread_ptr->ptr_->joinable()){
        thread_ptr->ptr_->detach();
    }
    {
        std::unique_lock<std::mutex> lck(m_mtx_threads);
        m_threads.emplace(id, std::move(thread_ptr));
    }
}

void ThreadPool::shutDown() {
    shutDown(false);
}
void ThreadPool::shutDownNow() {
    shutDown(true);
}
void ThreadPool::shutDown(bool is_now) {
    if(is_now){
        this->is_shutdown_now.store(true);
    }else{
        this->is_shutdown.store(true);
    }
    m_cond.notify_all();
}

int ThreadPool::getWaitingThread() {
    return this->thread_waiting_num;
}

int ThreadPool::getTotalThread() {
    return this->thread_existing_num.load();
}



