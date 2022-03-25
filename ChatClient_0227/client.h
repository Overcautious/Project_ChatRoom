//#ifndef CLIENT_H
//#define CLIENT_H
//#include <QTcpSocket>
//#include <QtAlgorithms>
//#include <string>
//#include <fstream>
//#include <QMessageBox>
//#include <QDebug>
//#include <QTextCodec>
//#include <QObject>
//#include <QJsonObject>
//#include <QJsonDocument>
//#include "protocolmsg.h"
//#include "dataparser.h"

//#define IMAGE_PATH "./image/"
//#define TCP_BUFSIZE 8192
//using namespace std;

//class Client
//{
//public:
//    QString hostName;
//    quint16 port;
//    QTcpSocket tcpClient;
//    char buffer[TCP_BUFSIZE];

//    unsigned int protocolId;
//    unsigned int account;
//    unsigned int dataType;
//    unsigned int dataLength;


//public:
//    Client();
//    void readHeadData();

//    QJsonObject readServerMsg();

//    void writeText(unsigned int account ,string text, unsigned int protocolId = SEND);

//    void closeTcpSocket();

//    QTcpSocket * getTcpClient();

//    ~Client();
//};

//#endif // CLIENT_H
