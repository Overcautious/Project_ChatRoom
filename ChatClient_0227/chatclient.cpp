#include "chatclient.h"

ChatClient::ChatClient()
{

}

ChatClient* ChatClient::chatClient = nullptr;
ChatClient* ChatClient::getTcpSocketClient(){
    if(chatClient == nullptr){
        chatClient = new ChatClient;
        chatClient->hostName = "1.117.220.169";
        chatClient->port = 8080;
        qDebug() << "[ChatClient::getTcpSocketClient] 正在连接服务器" << chatClient->port;
        chatClient->tcpClient.connectToHost(chatClient->hostName, chatClient->port);
        qDebug() << chatClient->tcpClient.waitForConnected(3000);
        qDebug() << chatClient->tcpClient.state();
        if(!chatClient->tcpClient.waitForConnected(5000)){
            qDebug() << "[ChatClient::getTcpSocketClient] 连接失败，超时！";
            return nullptr;
        }else{
            qDebug() << "[ChatClient::getTcpSocketClient] 连接成功";
        }
    }
    return chatClient;
}

ChatClient* ChatClient::reconnect(int account, string password){
    chatClient = nullptr;
    getTcpSocketClient();

    if(chatClient->ReConnected(account, password))
        qDebug() << "重连服务器成功";

    return chatClient;
}


bool ChatClient::ReConnected(int account, string password){
    QJsonObject loginInfo;
//    qDebug() << "m_account = " << m_account;

    loginInfo.insert("account", account);
    loginInfo.insert("password", QString::fromStdString(password));
    QJsonDocument doc;
    doc.setObject(loginInfo);
    string loginMsg = doc.toJson(QJsonDocument::Compact).toStdString();

    chatClient->writeText(account, loginMsg, RECONNECT);
    chatClient->getTcpClient()->waitForReadyRead(-1);
    chatClient->receiveData();
    qDebug() << "收到后台的重连确认信息";

    QString Msg = QString::fromStdString(chatClient->getReconnectMsg());
    QJsonObject ReconnectRespon = QJsonDocument::fromJson(Msg.toUtf8()).object();

    int statue = ReconnectRespon["statue"].toInt();
    qDebug() << ReconnectRespon["statue"];
    if(statue == RECONNECT_SUCCESS){
        qDebug() << "重连成功";
        return true;

    }else{
        qDebug() << "重连失败";
        return false;
    }
}

//bool ChatClient::readData(){
//    readHeadData(); //读取数据头
//    content = "";
//    if(dataType == TEXT){
//        readTextContent();
//        return true;
//    }else if(dataType == IMAGE){
//        readImageContent();
//        return true;
//    }else{
//        return false;
//    }
//}

void ChatClient::receiveData(){
    int ero;
    m_inBuffer.readFd(tcpClient, &ero); // 一次性接受所有数据
    while(m_inBuffer.readableBytes() > 0){
        readData_();
    }
}

// 从m_inBuffer中读取头部信息
bool ChatClient::readHeadData_(){
    if(m_inBuffer.readableBytes() <= 0)
        return false;
    string headData = m_inBuffer.retrieveAsString(SIZE_BUFFER_HEAD);
    headData.c_str();
    DataParser dp((char*)headData.data());

    dp.baseParse(); // 解析头部
    protocolId = dp.getProtocolId();
    account = dp.getAccount();
    dataType = dp.getDataType();
    dataLength = dp.getDataLength();
    qDebug("[Client::readHeadData] readHeadData");
    qDebug() << protocolId << account << dataType << dataLength;
    return true;
}
bool ChatClient::readData_(){
    readHeadData_(); //读取数据头
//    if(!content.empty())
//        // sleep，等待消息处理

    if(dataType == TEXT){
        readTextContent_();
        return true;
    }else if(dataType == IMAGE){
        readImageContent();
        return true;
    }else{
        return false;
    }
}
void ChatClient::readTextContent_(){
    qDebug("ChatClient::readTextContent_()\n");
    content = m_inBuffer.retrieveAsString(dataLength);
    switch (protocolId) {
        case SEND: {
            emit textDisplayAble();
            break;
        }
        case NOTICE: {  // 某某走近了、离开了 聊天室
            notice = content;
            content.clear();
            emit noticeDisplayAble();
            break;
        }
        case ONLINELIST :{
            emit onlineDisplayAble();
            break;
        }
        case LOGIN:{    // 登录的状态返回信息
            loginMsg = content;
            content.clear();
            break;
        }
        case RECONNECT:{
            reconnectMsg = content;
            content.clear();
            break;
        }
    }
}


void ChatClient::readImageContent(){
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy-MM-dd-hh-mm-ss-zzz");
    imagePath = IMAGE_PATH + current_date.toStdString() + ".png";

    ofstream os(imagePath, ios::out | ios::binary | ios::app);
    while(tcpClient.waitForReadyRead(-1)){
        unsigned int size = 0;
        unsigned int bufSize = 0;
        while(dataLength != 0 && tcpClient.bytesAvailable()){
            qDebug() << "[ChatClient::readImageContent] 可读取的字节数为" << tcpClient.bytesAvailable();
            size = tcpClient.read(buffer, min(bufSize, dataLength));
            dataLength -= size;
            os.write(buffer, size); //写入图片
            qDebug() << "[ChatClient::readTextContent] 剩余读取的字节数为" << dataLength;
        }
        if(dataLength == 0){
            break;
        }
    }
    os.close();
    emit imageDisplayAble();
    qDebug() << "[ChatClient::readTextContent] 图片读取完毕";
}

//QJsonObject ChatClient::readJsonMsg(){
//    readHeadData();
//    qDebug("ChatClient::readJsonMsg()\n");
//    string serverMsg;
//    unsigned int size = 0;
//    unsigned int bufsize = TCP_BUFSIZE;
//    qDebug() << "[ChatClient::readJsonMsg] 需要读取的字节数为" << dataLength;

//    while(dataLength != 0 && tcpClient.bytesAvailable()){
//       qDebug() << "[ChatClient::readJsonMsg] 可读取的字节数为" << tcpClient.bytesAvailable();
//       size = tcpClient.read(buffer, min(bufsize, dataLength));

//       dataLength -= size;
//       serverMsg = serverMsg + string(buffer, size);
//       qDebug() << "[ChatClient::readJsonMsg] 剩余读取的字节数为" << tcpClient.bytesAvailable();
//    }

//    QString Msg = QString::fromStdString(serverMsg);
//    QJsonObject rhs = QJsonDocument::fromJson(Msg.toUtf8()).object();
//    return rhs;
//}

void ChatClient::writeText(unsigned int account, string text, unsigned int protocolId){
    DataEncoder encoder;
    qDebug() << "[Client::writeText] data length:" << text.length();
    string headStr = encoder.encode(protocolId, account, TEXT, text.length());
    size_t send_size = tcpClient.write(headStr.data(), headStr.length());
    if(send_size != headStr.length()){
        qDebug() << "发送信息失败！";
    }
    qDebug() << "[Client::writeText] send headStr successful:" << send_size;

    if(text.length() != 0){
        tcpClient.write(text.data(), text.length());
        qDebug() << "[Client::writeText] send text successful:" << text.length();
    }
}

bool ChatClient::Heartbeat(){
    DataEncoder encoder;

    string headStr = encoder.encode(ACK, account, TEXT, 0);
    size_t send_size = tcpClient.write(headStr.data(), headStr.length());
    if(send_size != headStr.length()){
        qDebug() << "发送心跳包失败！";
        return false;
    }else{
        qDebug() << "发送心跳包成功！";
        return true;
    }
}


void ChatClient::closeTcpSocket(){
    tcpClient.disconnectFromHost();
    tcpClient.close();
}
QTcpSocket * ChatClient::getTcpClient(){
    return &tcpClient;
}
ChatClient::~ChatClient(){
    delete chatClient;
    chatClient = nullptr;
}
