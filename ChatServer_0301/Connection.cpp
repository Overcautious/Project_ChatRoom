//
// Created by zy on 2022/1/9.
//

#include "Connection.h"

    Connection::Connection(Server* ServerPtr, int fd, int ServerThreadId):m_ServerPtr(ServerPtr), m_fd(fd), m_ServerThreadId(ServerThreadId) {

    }
    Connection::~Connection() {
        // delete m_ServerPtr;
        close(m_fd);
    }

    int Connection::readData(int *savedErrno) {
        int ret = m_inBuffer.readFd(m_fd, savedErrno);
        return ret;
    }

    int Connection::writeData(int *savedErrno) {
        int ret = m_outBuffer.writeFd(m_fd, savedErrno);
        return ret;
    }

    std::string Connection::parseMessage() {

        return m_inBuffer.retrieveAsString();
    }


    bool Connection::isInServerThread() {
        return m_ServerThreadId == gettid();
    }

    int Connection::sendMessage() {
        int* savedErrno;
        int ret = m_outBuffer.writeFd(m_fd, savedErrno);
        return ret;

    }

    void Connection::appendOutBuffer(std::string str) {
        m_outBuffer.append(str);
    }

    int Connection::writableBytes() {
        return m_outBuffer.readableBytes();
    }

    void Connection::parseHead() {
        msg_protocolId = m_inBuffer.parseInt(SIZE_PROTOCOL) ;
        msg_account = m_inBuffer.parseInt(SIZE_ACCOUNT) ;
        msg_dataType = m_inBuffer.parseInt(SIZE_TYPE) ;
        msg_dataLength = m_inBuffer.parseInt(SIZE_DATA) ;
    }

    std::string Connection::readTextContent(size_t len) {
        return m_inBuffer.retrieveAsString(len);
    }

void Connection::set_userInfo(const int &account, const std::string &username, const std::string &loginTime) {
    m_account = account;
    m_username = username;
    m_loginTime_Str = loginTime;
}


