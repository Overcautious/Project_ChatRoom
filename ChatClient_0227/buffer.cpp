#include "buffer.h"
// 从套接字读到缓冲区
ssize_t Buffer::readFd(QTcpSocket& fd, int *savedError){
    char extraBuf[65536];
    const size_t writable = writableBytes();


    qDebug() << "[ChatClient::readTextContent] 可读取的字节数为" << fd.bytesAvailable();
    size_t datalength = fd.bytesAvailable();
    char* buffer = getBufferBegin() + m_writeIndex;;

    unsigned int size = fd.read(buffer, qMin(writable, datalength));
    m_writeIndex += size;
    size_t datalength_ = 0;
    if(size < datalength){
        qDebug() << "[ChatClient::readTextContent] 剩余读取的字节数为" << fd.bytesAvailable();
        datalength_ = fd.bytesAvailable();
        fd.read(extraBuf, datalength_);
        append(extraBuf, datalength_);
    }
    return datalength + datalength_;
}

// 从缓冲区写入套接字
ssize_t Buffer::writeFd(QTcpSocket fd, int *savedError){
    size_t nLeft = readableBytes();
    char *bufPtr = getBufferBegin() + m_readIndex;
//    tcpClient.write(headStr.data(), headStr.length());
//    fd.write(bufPtr, nLeft)
    ssize_t n;
    if ((n = fd.write(bufPtr, nLeft) <= 0)) {
        if (n < 0 && n == EINTR){
            return 0;
        }
        else if(n == 0){
            qDebug() << "发送长度为0！";
        }
        else {
            printf("[Buffer:writeFd] write error: %s\n", strerror(errno));
            *savedError = errno;
            return -1;
        }
        } else {
        m_readIndex += n;
        return n;
        }
}
