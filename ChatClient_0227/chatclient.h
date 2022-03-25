#ifndef CHATCLIENT_H
#define CHATCLIENT_H
#include <QObject>
#include <QTextCodec>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDateTime>
#include <QTcpSocket>
#include <QtAlgorithms>
#include <string>
#include <fstream>
#include <QMessageBox>
#include <QDebug>

#include "dataparser.h"
#include "dataencoder.h"
// #include "client.h"
#include "buffer.h"
using namespace std;

#define IMAGE_PATH "./image/"
#define TCP_BUFSIZE 8192


class ChatClient:public QObject
{
    Q_OBJECT
private:
    Buffer m_inBuffer;
    Buffer m_outBuffer;


    QString hostName;
    quint16 port;
    QTcpSocket tcpClient;
    char buffer[TCP_BUFSIZE];

    unsigned int protocolId;
    unsigned int account;
    unsigned int dataType;
    unsigned int dataLength;

    unsigned int m_account;
    string m_username;
    string m_password;

public:
    ChatClient();
    ~ChatClient();
    static ChatClient* chatClient;  // 单例指针

    string content; // 数据
    string notice;
    string imagePath; //  图片保存路径
    string loginMsg;
    string reconnectMsg;



public:
    static ChatClient* getTcpSocketClient();
    static ChatClient* reconnect(int account, string password);
    void receiveData();
//    bool readData();
    bool readHeadData_();
    bool readData_();
    void readTextContent_();
    void makeContentEmpty(){
        content.clear();
    }
//    void readTextContent();
    void closeTcpSocket();
    void readImageContent();
    bool Heartbeat();
    bool ReConnected(int account, string password);

    QTcpSocket * getTcpClient();
    QJsonObject readJsonMsg();
    void writeText(unsigned int account, string text, unsigned int protocolId);
    void setUserInfo(unsigned int account, string username){
        m_account = account;
        m_username = username;
    }
    void setUserInfo(unsigned int account, string username, string password){
        m_account = account;
        m_username = username;
        m_password = password;
    }
    unsigned int getAccount(){
        return m_account;
    }

    string getUsername(){
        return m_username;
    }
    string getContent(){
        return content;
    };
    string getNotice(){
        return notice;
    }
    string getImagePath(){
        return imagePath;
    }
    string getLoginMsg(){
        return loginMsg;
    }
    string getReconnectMsg(){
        return reconnectMsg;
    }
    string getPassword(){
        return m_password;
    }
// 信号函数
signals:
    void textDisplayAble();
    void imageDisplayAble();
    void noticeDisplayAble();
    void onlineDisplayAble();
};

#endif // CHATCLIENT_H
