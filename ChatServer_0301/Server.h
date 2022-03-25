//
// Created by zy on 2022/1/9.
//

#ifndef CHATSERVER_SERVER_H
#define CHATSERVER_SERVER_H

#include "all.h"
#include "Connection.h"
#include "Epoll.h"
#include "ThreadPool.h"
#include "DataEncoder.h"
#include "UserManage.h"
#include "LRUCache.h"

    class Epoll;
    class LRUCache;
    class Connection;
    class ThreadPool;
    class DataEncoder;
    //class UserManage;
    //typedef std::function<void()> Functor_;
    using Functor = std::function<void()>;


class Server {
    private:

    int m_lsnFd;    // 监听套接字
    int m_port;     // 端口
    int m_threadID; // 运行线程号
    std::unique_ptr<Epoll> m_epoll;

    UserManage m_UserManage;
//    std::unordered_map<int, Connection*> m_online_user;
//    std::set<Connection*> m_connections;     // 保存所有的连接
    std::unique_ptr<LRUCache> m_online_cache;

    std::vector<Functor> m_FunctorList;      // 待执行函数 读写相关

    std::mutex m_Func_mtx;
    std::mutex m_cnn_mtx;
    std::mutex m_online_mtx;

    std::unique_ptr<Connection> m_lsnCnn;    // 自身对应的cnn 。防止指针被拷贝，保持其唯一性
    std::shared_ptr<ThreadPool> m_threadPool;   // 线程池的指针，由于要传递给其他函数，因此设置为shared指针
public:
    Server(u_short port , int numWorkers);
    ~Server();

    int createListenFd(u_short port);

    [[noreturn]] void run();

    void acceptConnection();                    // 建立连接
    void closeConnection(Connection* cnn);   // 关闭连接
    void getRequestInfo(Connection* cnn);        // 接受某用户发送的信息
    void sendResponseInfo(Connection* cnn);                         // 发送信息

    void addFunctor(Functor cb);
    void doPendingFunctors();


    void sendTextToAllUser(unsigned int account, std::string message, unsigned int protocolId = SEND);
    void sendTextToOneUser(unsigned int account, std::string message, unsigned int protocolId = SEND);
    void sendTextToOneUser_(Connection* cnn, std::string message, unsigned int protocolId = SEND);
    void updateOnlineList();
    void removeOnline(int account);
};


#endif //CHATSERVER_SERVER_H
