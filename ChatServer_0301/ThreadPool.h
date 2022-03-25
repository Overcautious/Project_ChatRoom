//
// Created by zy on 2022/1/14.
//

#ifndef CHATSERVER_THREADPOOL_H
#define CHATSERVER_THREADPOOL_H
#include "all.h"
////
//class ThreadPool {
//public:
//    using JobFunction = std::function<void()>;
//
//    ThreadPool(int numWorkers);
//    ~ThreadPool();
//
//    void pushJob(const JobFunction& job);
//private:
//    std::vector<std::thread> m_threads;
//    std::mutex m_mtx;
//    std::condition_variable m_cond;
//    std::queue<JobFunction> m_jobs;
//    bool m_isClose;
//};

#include <vector>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <memory>
#include <sstream>
#include <string>
#include <unistd.h>
#include <iostream>
#include <unordered_map>
#include <condition_variable>
#include <sys/syscall.h>
#include <atomic>
using JobFunction = std::function<void()>;

enum class ThreadState {e_init = 0, e_waiting = 1, e_running = 2, e_stop = 3};
enum class ThreadFlag {e_init = 0, e_core = 1, e_cache = 2};
/*
 * enum class 的作用是限定作用域，防止命名冲突
 * */
using ThreadPtr = std::shared_ptr<std::thread>;
using ThreadId = std::atomic<int>;
using ThreadStateAtomic = std::atomic<ThreadState>;
using ThreadFlagAtomic = std::atomic<ThreadFlag>;


struct ThreadWrapper{
    ThreadPtr ptr_;
    ThreadId id_;
    ThreadStateAtomic state_;
    ThreadFlagAtomic flag_;
    ThreadWrapper(){
        ptr_ = nullptr;
        id_.store(0);
        state_.store(ThreadState::e_init);
        flag_.store(ThreadFlag::e_init);
    }
};

using ThreadWrapperPtr = std::shared_ptr<ThreadWrapper>;



class ThreadPool {
public:
    void AddThread();   // 添加线程

    void shutDown();    // 关闭线程池，等待未结束任务与队列任务
    void shutDownNow();     // 立即关闭线程池
    //void RESIZE(int threadNum);

    ThreadPool(int numCoreWorkers);
    ThreadPool(int numCoreWorkers, int numMaxWorkers, std::chrono::seconds sleepTime = std::chrono::seconds(60));
    void run();
    void runAsSingle();
    ~ThreadPool();

    int getWaitingThread();
    int getTotalThread();

    void pushJob(const JobFunction& job);
private:
    void shutDown(bool is_now);
    void AddThread(int id, ThreadFlag thread_flag);

    std::atomic<bool> is_shutdown;
    std::atomic<bool> is_shutdown_now;

    //std::atomic<int> thread_working_num;
    std::atomic<int> thread_waiting_num;
    std::atomic<int> thread_existing_num; // 当前存在多少线程

    std::atomic<int> thread_id;

    int m_coreNumWorkers;   // 核心线程数
    int m_MaxNumWorkers;    // 最大线程数

    std::chrono::seconds m_sleepTime;        // 非核心线程的等待时间，超过该时间则进行线程回收

    std::unordered_map<int, ThreadWrapperPtr> m_threads;
    //std::vector<ThreadWrapperPtr> m_threads;
    std::mutex m_mtx;
    std::mutex m_mtx_threads;
    std::condition_variable m_cond;
    std::queue<JobFunction> m_jobs;
};



#endif //CHATSERVER_THREADPOOL_H
