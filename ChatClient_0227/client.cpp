//#include "client.h"
//#include "dataencoder.h"

//Client::Client()
//{

//}

//void Client::readHeadData(){
//    int size = tcpClient.read(buffer, SIZE_BUFFER_HEAD);  // 读取头部的八个字节
//    if(size < 0){
//        qDebug("[Client::readHeadData] read errno\n");
//    }
//    DataParser dp(buffer);
//    dp.baseParse(); // 解析头部
//    protocolId = dp.getProtocolId();
//    account = dp.getAccount();
//    dataType = dp.getDataType();
//    dataLength = dp.getDataLength();
//    qDebug("[Client::readHeadData] readHeadData");
//    qDebug() << protocolId << account << dataType << dataLength;
//}

//QJsonObject Client::readServerMsg(){
//    readHeadData();
//    tcpClient.read(buffer, dataLength);
//    QString serverMsg = QString::fromStdString(string(buffer, dataLength));
//    QJsonObject serverInfoObj = QJsonDocument::fromJson(serverMsg.toUtf8()).object();
//    return serverInfoObj;
//}

//void Client::writeText(unsigned int account, string text, unsigned int protocolId){
//    DataEncoder encoder;
//    qDebug() << "[Client::writeText] data length:" << text.length();
//    string headStr = encoder.encode(protocolId, account, TEXT, text.length());
//    size_t n_ = tcpClient.write(headStr.data(), headStr.length());
//    if(n_ == 0){
//        qDebug() << "发送的消息长度为0";
//    }
//    qDebug() << "[Client::writeText] send headStr successful:" << headStr.length();
//    qDebug() << n_;
//    if(text.length() != 0){
//        tcpClient.write(text.data(), text.length());
//        qDebug() << "[Client::writeText] send text successful:" << text.length();
//    }
//}

//QTcpSocket * Client::getTcpClient(){
//    return &tcpClient;
//}

//void Client:: closeTcpSocket(){
//    tcpClient.disconnectFromHost();
//    tcpClient.close();
//}

//Client::~Client(){

//}
