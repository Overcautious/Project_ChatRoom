//
// Created by zy on 2022/1/9.
//

#include "Server.h"
#include "config.h"
#include <functional>

    Server::Server(u_short port, int numWorkers):m_threadID(gettid()),
        m_lsnCnn(static_cast<Connection *>(new Connection(this, createListenFd(port), m_threadID))),
        m_epoll(new Epoll()), m_threadPool(new ThreadPool(numWorkers) ), m_online_cache(new LRUCache(200)){
            //m_UserManage =
            assert(m_lsnFd >= 0);
    }
    Server::~Server() {

    }

    int Server::createListenFd(u_short port) {
        port = ((port <= 1024) || (port >= 65535)) ? PORT : port;
        m_port = port;

        // 创建套接字
        if((m_lsnFd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) == -1){
            printf("[Server::Server] lsnFd = %d socket: %s\n", m_lsnFd, strerror(errno));
            ::close(m_lsnFd);
            exit(-1);   //退出
        }
        // 设置端口复用
        int optVal = 1;
        if(::setsockopt(m_lsnFd, SOL_SOCKET, SO_REUSEADDR, (const void*)&optVal, sizeof(int)) == -1){
            printf("[Server::Server] lsnFd = %d setsockopt: %s\n", m_lsnFd, strerror(errno));
            ::close(m_lsnFd);
            exit(-1);   //退出
        }

        struct sockaddr_in addr;
        ::bzero((char*)&addr, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = ::htons(m_port);

        if(::bind(m_lsnFd, (struct sockaddr*)&addr, sizeof(addr)) == -1){
            printf("[Utils::createListenFd] lsnFd = %d setsockopt: %s\n", m_lsnFd, strerror(errno));
            ::close(m_lsnFd);
            exit(-1);   //退出
        }

        // 开始监听，监听队列最大为LSTNQUE
        if (::listen(m_lsnFd, MAX_CONNECTIONS) == -1)
        {
            printf("[Utils::createListenFd] lsnFd = %d listen: %s\n", m_lsnFd, strerror(errno));
            exit(-1);   //退出
        }

        if(m_lsnFd == -1){
            ::close(m_lsnFd);
            exit(-1);   //退出
        }
        printf("[Server::Server] lsnFd = %d \n", m_lsnFd);
        return m_lsnFd;
    }

    [[noreturn]] void Server::run() {
        m_epoll->add(m_lsnFd, m_lsnCnn.get(), EPOLLIN | EPOLLET);
        m_epoll->setConnectionCallbak(std::bind(&Server::acceptConnection, this));
        m_epoll->setCloseConnectionCallbak(std::bind(&Server::closeConnection, this, std::placeholders::_1));
        m_epoll->setGetRequestInfoCallbak(std::bind(&Server::getRequestInfo, this, std::placeholders::_1));
        m_epoll->setSendResponseInfoCallbak(std::bind(&Server::sendResponseInfo, this, std::placeholders::_1));

        while(true){
            int eventsNum = m_epoll->wait();
            if(eventsNum > 0){
                m_epoll->handleEvent(m_lsnFd, m_threadPool ,eventsNum);
            }

            doPendingFunctors();
//            std::cout << "m_online_user.size:" << m_online_user.size() << std::endl;
            std::cout << "m_online_cache.size:" << m_online_cache->size() << std::endl;
        }
    }

    void Server::closeConnection(Connection *cnn) {

        int fd = cnn->getFd();
        m_epoll->del(fd, cnn, 0);
        //m_connections.erase(cnn);
        delete cnn;
        cnn = nullptr;
        return;
    }

    void Server::acceptConnection() {
        while(true){
            int acceptFd = ::accept4(m_lsnFd, nullptr, nullptr, SOCK_NONBLOCK | SOCK_CLOEXEC);  // 非阻塞

            if(acceptFd == -1){
                if(errno == EAGAIN)
                    break;
                printf("[ChatServer::acceptConnection] accept: %s\n", strerror(errno));
                break;
            }
            Connection* cnn = new Connection(this, acceptFd, m_threadID);
            m_epoll->add(acceptFd, cnn, (EPOLLIN | EPOLLONESHOT));
//            {
//                std::unique_lock<std::mutex> lck(m_cnn_mtx);    // 将该连接添加至队列中
//                m_connections.insert(cnn);
//            }
            printf("[ChatServer::acceptConnection] \n");
        }
    }

    void Server::getRequestInfo(Connection *cnn) {
        if(cnn == nullptr)
            return;
        assert(cnn != nullptr);
        int fd = cnn->getFd();

        int rdErrno;
        int retRead = cnn->readData(&rdErrno);

        // 没有数据
        if(retRead == 0){
            closeConnection(cnn);
            return ;
        }

        if(retRead < 0 && rdErrno != EAGAIN){
            closeConnection(cnn);
            return ;
        }

        if(retRead < 0 && rdErrno == EAGAIN){
            m_epoll->mod(fd, cnn, (EPOLLIN | EPOLLONESHOT));  // 需要重新注册检测可读事件
            return;
        }

        /*
         * cnn->get 检查数据，然后根据数据报首部关键字，判断执行操作
         * 1. 检查数据完整性，如果数据不完整，则改为EPOLLIN，且不改变缓冲器的上下界标(暂未完成)
         *
         * */

        cnn->parseHead();
        unsigned int protocolId =cnn->getProtocolId();
        unsigned int account = cnn->getAccount();
        unsigned int dataType = cnn->getDataType();
        unsigned int dataLength = cnn->getDataLength();

        std::cout << "-------------------------getRequestInfo-----------------------" << std::endl;
//        std::cout << "m_online_cache的地址：" << m_online_cache << std::endl;
//        std::cout << "protocolId:" << protocolId << std::endl;
//        std::cout << "account:" << account << std::endl;
//        std::cout << "dataType:" << dataType << std::endl;
//        std::cout << "dataLength:" << dataLength << std::endl;

        switch (protocolId) {
            case LOGIN:{
                if(is_debug)
                    std::cout << "[Server::getRequestInfo] LOGIN" << std::endl;
                std::string message = cnn->readTextContent(dataLength);
                Json::Reader JsonReader;
                Json::Value JsonValue;
                JsonReader.parse(message, JsonValue);
                std::string registerAccount = JsonValue["account"].asString();
                std::string registerPassword = JsonValue["password"].asString();
                std::pair<int, std::string> userInfo = m_UserManage.checkLogin(registerAccount, registerPassword);
                // 返回登陆状态信息
                Json::Value loginResult;
                std::string loginTime = MyTime::getCurrentFormatTimeStr();
                if(userInfo.first != 0 && m_online_cache->get((int)account) == nullptr){
                    // 查询成功，数据库中包含该用户, 且在线用户中不包含该用户
                    // 设置*cnn中的登陆信息：账户，账户名，登陆时间

                    cnn->set_userInfo((int)account, userInfo.second, loginTime);
                    {
                        std::unique_lock<std::mutex> lck(m_online_mtx);
                        m_online_cache->put((int)account, cnn); // 添加在线用户
                    }
//                    m_online_user.insert({(int)account, cnn});  // 添加在线用户

                    loginResult["statue"] = LOGIN_SUCCESS;
                    loginResult["username"] = userInfo.second;
                    printf("[Server::getRequestInfo] 登陆成功\n");


                }else if(userInfo.first == 0){
                    // 登陆失败，数据库中无此数据
                    loginResult["statue"] = LOGIN_FAIL;
                    printf("[Server::getRequestInfo] 登陆失败\n");
                }else{
                    loginResult["statue"] = LOGIN_EXIST;
                    printf("[Server::getRequestInfo] 登陆失败\n");
                }

                std::string loginResultStr = loginResult.toStyledString();
                printf("[Server::getRequestInfo] loginResultStr = %s \n", loginResultStr.data());
                addFunctor(std::bind(&Server::sendTextToOneUser_, this, cnn, loginResultStr, LOGIN));
                m_epoll->mod(fd, cnn, (EPOLLIN | EPOLLOUT | EPOLLONESHOT));

                addFunctor(std::bind(&Server::updateOnlineList, this)); // 更新在线列表

                break;
            }
            case READ:{
                if(is_debug)
                    std::cout << "[Server::getRequestInfo] READ" << std::endl;
                // 登陆后的提示
                std::string read_name = cnn->get_mUsername();
                std::string read_account = std::to_string(cnn->get_mAccount());
                std::string read_loginTime = cnn->get_mLoginTime();
                {
                    std::unique_lock<std::mutex> lck(m_online_mtx);
                    m_online_cache->put(cnn->get_mAccount(), cnn); // 添加在线用户
                }


                std::string noticeStr = "用户" + read_name + "(" + read_account + ")走进了聊天室";
                addFunctor(std::bind(&Server::sendTextToAllUser, this, cnn->get_mAccount(), noticeStr, NOTICE)); // 发给所有用户
//                std::string onlineStr = read_account + "\t" + read_name + "\t" + read_loginTime;
//                addFunctor(std::bind(&Server::sendTextToAllUser, this, cnn->get_mAccount(), onlineStr, ONLINELIST)); // 发给所有用户
                m_epoll->mod(fd, cnn, (EPOLLIN | EPOLLOUT | EPOLLONESHOT));
                addFunctor(std::bind(&Server::updateOnlineList, this)); // 更新在线列表
                break;
            }
            case REGISTER:{
                if(is_debug)
                    std::cout << "[Server::getRequestInfo] REGISTER" << std::endl;
                std::string RegisterMsg = cnn->readTextContent(dataLength);
                Json::Reader JsonReader;
                Json::Value JsonValue;
                JsonReader.parse(RegisterMsg, JsonValue);
                std::string account = JsonValue["account"].asString();
                std::string username = JsonValue["username"].asString();
                std::string password = JsonValue["password"].asString();
                Json::Value registerResult;
                if(m_UserManage.isRegistered(account) || !m_UserManage.registerUser(account, username, password)){
                    registerResult["statue"] = REGISTER_FAIL;
                }else{
                    registerResult["statue"] = REGISTER_SUCCESS;
                }
                std::string registerResultStr = registerResult.toStyledString();
                addFunctor(std::bind(&Server::sendTextToOneUser_, this, cnn, registerResultStr, SEND));
                break;
            }
            case SEND:{
                if(is_debug)
                    std::cout << "[Server::getRequestInfo] SEND" << std::endl;
                 std::string baseMsg = "(" + std::to_string(account) + ")说：";
                 addFunctor(std::bind(&Server::sendTextToAllUser, this, account, baseMsg, SEND));
                 if(dataType == TEXT){
                    std::string message = cnn->readTextContent(dataLength);

                    addFunctor(std::bind(&Server::sendTextToAllUser, this, account, message, SEND));   // 发送给所有用户
                 }else if(dataType = IMAGE){

                 }
                 m_epoll->mod(fd, cnn, (EPOLLIN | EPOLLOUT | EPOLLONESHOT));
                 break;
            }
            case LOGOUT:{
                if(is_debug)
                    std::cout << "[Server::getRequestInfo] LOGOUT" << std::endl;
                Connection* ptr = m_online_cache->get(account);
                if(ptr == nullptr){
                    // 在online_cache中找不到该账户，所以仅在
                    if(is_debug)
                        std::cout << "[Server::getRequestInfo] 在online_cache中找不到该账户，仅移除cnn*" << std::endl;
                    addFunctor(std::bind(&Server::closeConnection, this, cnn));
                    break;
                }

                std::string username = ptr->get_mUsername();


                addFunctor(std::bind(&Server::removeOnline, this, account));
                addFunctor(std::bind(&Server::closeConnection, this, cnn)); // 删除链接

                printf("[Server::getRequestInfo] 用户 %s  LOGOUT\n", username.data());
                std::string noticeStr = "用户" + username + "(" + std::to_string(account) + ")离开了聊天室";


                if(m_online_cache->size() > 1){
                    std::cout << "-------\n";
                    addFunctor(std::bind(&Server::sendTextToAllUser, this, account, noticeStr, NOTICE)); // 发给所有用户
                    addFunctor(std::bind(&Server::updateOnlineList, this)); // 更新在线列表
                }
                break;
            }

            case CLOSE:{
                break;
            }
            case RECONNECT:{
                if(is_debug)
                    std::cout << "[Server::getRequestInfo] RECONNECT" << std::endl;
                std::string message = cnn->readTextContent(dataLength);
                Json::Reader JsonReader;
                Json::Value JsonValue;
                JsonReader.parse(message, JsonValue);
                std::string registerAccount = JsonValue["account"].asString();
                std::string registerPassword = JsonValue["password"].asString();

                // 返回登陆状态信息
                Json::Value reconnectResult;
                std::cout << "[Server::getRequestInfo] RECONNECT account=" << (int)account << std::endl;
                Connection* ptr =  m_online_cache->get((int)account);
                if(ptr != nullptr){
                    if(is_debug)
                        std::cout << "[Server::getRequestInfo] RECONNECT 重连成功" << std::endl;
                    cnn->set_userInfo((int)account, ptr->get_mUsername(), ptr->get_mLoginTime());

                    {
                        std::unique_lock<std::mutex> lck(m_online_mtx);
                        m_online_cache->put((int) account, cnn);
                    }
//                    addFunctor(std::bind(&Server::closeConnection, this, ptr));
                    reconnectResult["statue"] = RECONNECT_SUCCESS;

                }else{
                    reconnectResult["statue"] = RECONNECT_FAIL;
                }



                std::string loginResultStr = reconnectResult.toStyledString();
                printf("[Server::getRequestInfo] reconnectResult = %s \n", loginResultStr.data());
                addFunctor(std::bind(&Server::sendTextToOneUser_, this, cnn, loginResultStr, RECONNECT));

                addFunctor(std::bind(&Server::sendTextToOneUser_, this, cnn, "123", SEND));
                m_epoll->mod(fd, cnn, (EPOLLIN | EPOLLOUT | EPOLLONESHOT));

                break;
            }
            case ACK:{
                m_epoll->mod(fd, cnn, (EPOLLIN | EPOLLONESHOT));
                m_online_cache->get((int)account);
                return ;
//                break;
            }
        }





        return;
    }

    void Server::sendResponseInfo(Connection* cnn){
        if(cnn == nullptr)
            return;
        assert(cnn != nullptr);
        int fd = cnn->getFd();
        int bytesToWrite = cnn->writableBytes();    //查询outBuffer中有多少可写字节

        if(bytesToWrite == 0){
            m_epoll->mod(fd, cnn, (EPOLLIN | EPOLLONESHOT));
            return;
        }
        int wrErrno;
        int retWrite = cnn->writeData(&wrErrno);   // 向套接字中写入

        if(retWrite < 0 && wrErrno ==EAGAIN){   // 写入失败，重新写入
            m_epoll->mod(fd, cnn, (EPOLLIN | EPOLLOUT | EPOLLONESHOT));
            return  ;
        }

        if(retWrite < 0 && wrErrno != EAGAIN){  // 写入失败，套接字失效，删除该链接
            closeConnection(cnn);
            return;
        }


        if(retWrite == bytesToWrite){   // 全部写入成功
            m_epoll->mod(fd, cnn, (EPOLLIN | EPOLLONESHOT));
        }else{  //只写入了一部分
            m_epoll->mod(fd, cnn, (EPOLLIN | EPOLLOUT | EPOLLONESHOT));
        }
    }

    void Server::addFunctor(Functor cb) {
        {
            std::unique_lock<std::mutex> lck(m_Func_mtx);
            m_FunctorList.push_back(cb);
        }
        // 唤醒函数
    }

    void Server::doPendingFunctors() {
        std::vector<Functor> functors;

        {
            std::unique_lock<std::mutex> lck(m_Func_mtx);
            if(m_FunctorList.empty())
                return;
            functors.swap(m_FunctorList);
        }

        for(const Functor& functor: functors){
            functor();
        }
    }



    //向所有的已连接套接字的outBuffer中写入message
    // 第一个参数是发送消息的account
    void Server::sendTextToAllUser(unsigned int account, std::string message, unsigned int protocolId) {    // protocolId默认为SEND
        {
            //std::unique_lock<std::mutex> lck(m_cnn_mtx);
            printf("[Server::sendTextToAllUser] \n" );
            std::unique_lock<std::mutex> lck(m_online_mtx);
            std::unordered_map<int, node*> map_ = m_online_cache->getMap();
            for(auto user : map_){
                Connection* cnn = m_online_cache->get(user.first);
                int fd = cnn->getFd();
                DataEncoder de;
                std::string MessageHead = de.encode(protocolId, account, TEXT, message.length());
                //std::string MessageHead = "";
                cnn->appendOutBuffer(MessageHead);
                cnn->appendOutBuffer(message);
                printf("[Server::sendTextToAllUser] send message = %s \n", message.c_str());
                m_epoll->mod(fd, cnn, (EPOLLIN | EPOLLOUT | EPOLLONESHOT));
            }
        }
    }
// 向某个用户发送
void Server::sendTextToOneUser(unsigned int account, std::string message, unsigned int protocolId) {
    std::unique_lock<std::mutex> lck(m_online_mtx);
    Connection* cnn = m_online_cache->get((int)account);
    int fd = cnn->getFd();
    DataEncoder de;
    std::string MessageHead = de.encode(protocolId, account, TEXT, message.length());
    cnn->appendOutBuffer(MessageHead);
    cnn->appendOutBuffer(message);
    printf("[Server::sendTextToOneUser] send message = %s \n", message.c_str());
    m_epoll->mod(fd, cnn, (EPOLLIN | EPOLLOUT | EPOLLONESHOT));
}

void Server::sendTextToOneUser_(Connection* cnn, std::string message, unsigned int protocolId) {

    int fd = cnn->getFd();
    DataEncoder de;
    std::string MessageHead = de.encode(protocolId, 1001, TEXT, message.length());
    cnn->appendOutBuffer(MessageHead);
    cnn->appendOutBuffer(message);
    printf("[Server::sendTextToOneUser] send message = %s \n", message.c_str());
    m_epoll->mod(fd, cnn, (EPOLLIN | EPOLLOUT | EPOLLONESHOT));
}

void Server::updateOnlineList() {

    std::unique_lock<std::mutex> lck(m_online_mtx);
    std::string onlineStr;
    std::unordered_map<int, node*> map_ = m_online_cache->getMap();
    for(auto user : map_){
        Connection* cnn = m_online_cache->get(user.first);
        if(onlineStr.size() > 0)
            onlineStr += "\n";
        onlineStr += std::to_string(cnn->get_mAccount()) + "\t" + cnn->get_mUsername() + "\t" + cnn->get_mLoginTime();
    }
    for(auto user : map_){
        Connection* cnn = m_online_cache->get(user.first);
        int fd = cnn->getFd();
        DataEncoder de;
        std::string MessageHead = de.encode(ONLINELIST, 0, TEXT, onlineStr.length());
        //std::string MessageHead = "";
        cnn->appendOutBuffer(MessageHead);
        cnn->appendOutBuffer(onlineStr);
        printf("[Server::sendTextToAllUser] send message = %s \n", onlineStr.c_str());
        m_epoll->mod(fd, cnn, (EPOLLIN | EPOLLOUT | EPOLLONESHOT));
    }

//
//    std::string onlineStr;
//    std::unique_lock<std::mutex> lck(m_cnn_mtx);
//    for(Connection* cnn : m_connections){
//        if(onlineStr.size() > 0)
//            onlineStr += "\n";
//        onlineStr += std::to_string(cnn->get_mAccount()) + "\t" + cnn->get_mUsername() + "\t" + cnn->get_mLoginTime();
//    }
//    for(Connection* cnn : m_connections){
//        int fd = cnn->getFd();
//        DataEncoder de;
//        std::string MessageHead = de.encode(ONLINELIST, 0, TEXT, onlineStr.length());
//        //std::string MessageHead = "";
//        cnn->appendOutBuffer(MessageHead);
//        cnn->appendOutBuffer(onlineStr);
//        printf("[Server::sendTextToAllUser] send message = %s \n", onlineStr.c_str());
//        m_epoll->mod(fd, cnn, (EPOLLIN | EPOLLOUT | EPOLLONESHOT));
//    }
}

void Server::removeOnline(int account) {
    std::unique_lock<std::mutex> lck(m_online_mtx);
    m_online_cache->remove(account);
}


