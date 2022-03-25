//
// Created by zy on 2022/1/9.
//


#include "Epoll.h"


    Epoll::Epoll():m_epollFd(epoll_create(EPOLL_CLOEXEC)), m_events(MAX_EVENTS) {
        assert(m_epollFd >= 0);
    }

    Epoll::~Epoll() {
        close(m_epollFd);
    }
    int Epoll::add(int fd, Connection *req, int evnts) {
        struct epoll_event ev;
        ev.data.ptr = req;
        ev.events = evnts;
        int ret = ::epoll_ctl(m_epollFd, EPOLL_CTL_ADD, fd, &ev);
        return ret;
    }

    int Epoll::mod(int fd, Connection *req, int evnts) {
        struct epoll_event ev;
        ev.data.ptr = req;
        ev.events = evnts;
        int ret = ::epoll_ctl(m_epollFd, EPOLL_CTL_MOD, fd, &ev);
        return ret;
    }

    int Epoll::del(int fd, Connection *req, int evnts) {
        struct epoll_event ev;
        ev.data.ptr = req;
        ev.events = evnts;
        int ret = ::epoll_ctl(m_epollFd, EPOLL_CTL_DEL, fd, &ev);
        return ret;
    }

    int Epoll::wait() {
        int eventsNum = ::epoll_wait(m_epollFd, &*m_events.begin(),
                                     static_cast<int>(m_events.size()), -1 );
        if(eventsNum == 0){
            printf("[Epoll::wait] nothing happen, epoll timeout\n");
        }else if(eventsNum < 0){
            printf("[Epoll:wait] epoll : %s\n", strerror(errno));
        }
        return eventsNum;
    }

    void Epoll::handleEvent(int lsnFd, std::shared_ptr<ThreadPool>& threadPool, int eventsNum) {
        assert(eventsNum > 0);
        for( int i=0; i<eventsNum; i++){
            Connection* cnn = (Connection*)(m_events[i].data.ptr);
            int fd = cnn->getFd();
            if(fd == lsnFd){
                c_buildConnection();
                //建立连接
            }else{
                // 进行处理套接字
                if ((m_events[i].events & EPOLLERR) ||
                    (m_events[i].events & EPOLLHUP) ||
                    (!m_events[i].events & EPOLLIN)){
                    // 套接字出了问题，应该关闭此套接字
                    c_closeConnection(cnn);
                }else if (m_events[i].events & EPOLLIN){
                    // 有数据写入
                    threadPool->pushJob(std::bind(c_getRequestInfoCallbak, cnn));
                    //c_getRequestInfoCallbak(cnn);
                }else if(m_events[i].events & EPOLLOUT){
                    threadPool->pushJob(std::bind(c_sendResponseInfoCallbak, cnn));
                    //c_sendResponseInfoCallbak(cnn);
                }else{
                    printf("[Epoll::handleEvent()] unexpected event\n");
                }
            }
        }
    }

