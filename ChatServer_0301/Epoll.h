//
// Created by zy on 2022/1/9.
//

#ifndef CHATSERVER_EPOLL_H
#define CHATSERVER_EPOLL_H
#include "all.h"
#include "Connection.h"
#include "ThreadPool.h"


    class Connection;
    class ThreadPool;

class Epoll {
private:
    using NewConnectionCallbak = std::function<void()>;
    using CloseConnectionCallbak = std::function<void(Connection*)>;
    using GetRequestInfoCallbak = std::function<void(Connection*)>;
    using SendResponseInfoCallbak = std::function<void(Connection*)>;


    int m_epollFd;
    std::vector<struct epoll_event> m_events;

    NewConnectionCallbak c_buildConnection;
    CloseConnectionCallbak c_closeConnection;
    GetRequestInfoCallbak c_getRequestInfoCallbak;
    SendResponseInfoCallbak c_sendResponseInfoCallbak;

public:

    Epoll();
    ~Epoll();

    // 修改监听事件
    int add(int fd, Connection* req, int evnts);
    int mod(int fd, Connection* req, int evnts);
    int del(int fd, Connection* req, int evnts);

    void setConnectionCallbak(const NewConnectionCallbak& cb){
        c_buildConnection = cb;
    }
    void setCloseConnectionCallbak(const CloseConnectionCallbak& cb){
        c_closeConnection = cb;
    }
    void setGetRequestInfoCallbak(const GetRequestInfoCallbak& cb){
        c_getRequestInfoCallbak = cb;
    }
    void setSendResponseInfoCallbak(const SendResponseInfoCallbak& cb){
        c_sendResponseInfoCallbak = cb;
    }

    int wait();
    void handleEvent(int lsnFd, std::shared_ptr<ThreadPool>& threadPool, int eventsNum);
};


#endif //CHATSERVER_EPOLL_H
