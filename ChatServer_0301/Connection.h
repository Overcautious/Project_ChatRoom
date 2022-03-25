//
// Created by zy on 2022/1/9.
//

#ifndef CHATSERVER_CONNECTION_H
#define CHATSERVER_CONNECTION_H
#include "all.h"
#include "Buffer.h"
#include "Server.h"


class Buffer;
class Server;
class Connection {
private:
    int m_fd;
    int m_ServerThreadId;
    Buffer m_inBuffer;    // 接受数据缓冲区
    Buffer m_outBuffer;   // 发送数据缓冲区



public:
    Server* m_ServerPtr;

    unsigned int msg_protocolId;
    unsigned int msg_account;
    unsigned int msg_dataType;
    unsigned int msg_dataLength;

//    struct user{
//        int account;
//        std::string username;
//        std::string loginTime;
//    };

    int m_account;
    std::string m_username;
    std::string m_loginTime_Str;
    long m_loginTime_Second;
    void set_userInfo(const int& account, const std::string& username, const std::string& loginTime);

    int get_mAccount(){
        return m_account;
    }
    std::string get_mUsername(){
        return m_username;
    }
    std::string get_mLoginTime(){
        return m_loginTime_Str;
    }

    unsigned int getProtocolId(){
        return msg_protocolId;
    }
    unsigned int getAccount(){
        return msg_account;
    }
    unsigned int getDataType(){
        return msg_dataType;
    }
    unsigned int getDataLength(){
        return msg_dataLength;
    }

    Connection(Server* ServerPtr, int fd, int ServerThreadId);
    Connection(){}
    ~Connection();
    int getFd(){
        return m_fd;
    }

    // buffer类相关
    void appendOutBuffer(std::string str);
    int readData(int *savedErrno);  //从套接字读取内容到inBuffer中
    int writeData(int *savedErrno); //向套接字写入outBuffer内容
    int writableBytes();


    bool isInServerThread();
    std::string parseMessage();
    void parseHead();   // 解析报头信息
    std::string readTextContent(size_t len);

    int sendMessage();


};



#endif //CHATSERVER_CONNECTION_H
