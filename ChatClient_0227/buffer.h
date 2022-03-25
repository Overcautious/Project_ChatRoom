#ifndef BUFFER_H
#define BUFFER_H
#include <vector>
#include <cassert>
#include <string>
#include <algorithm>
#include <QTcpSocket>
#include <QtAlgorithms>

#define INIT_SIZE 1024

class Buffer {
private:
    std::vector<char> m_buffer;
    size_t m_readIndex;
    size_t m_writeIndex;
public:
    Buffer():m_buffer(1024), m_readIndex(0), m_writeIndex(0){
        assert(readableBytes() == 0);
        assert(writableBytes() == INIT_SIZE);
    };
    ~Buffer(){}

    size_t readableBytes() const{       // 可读字节数
        return m_writeIndex - m_readIndex;
    }
    size_t  writableBytes() const{      // 可写字节数
        return m_buffer.size() - m_writeIndex;
    }

    size_t prependableBytes() const{    // m_readIndex前的空闲缓冲区
        return m_readIndex - 0;
    }

    char* beginWrite(){                 // 可写char指针
        return getBufferBegin() + m_writeIndex;
    }

    const char* beginWrite() const{                 // 可写char指针
        return getBufferBegin() + m_writeIndex;
    }

    const char* peek() const{           // 第一个可读位置
        return getBufferBegin() + m_readIndex;
    }

    void retrieve(size_t len){          // 取出len个字节
        assert(len <= readableBytes());
        m_readIndex += len;
    }

    void retrieveUnitil(const char* end){       // 一直取出到end字符
        assert(peek() <= end);
        assert(end <= beginWrite());
        retrieve(end - peek());
    }

    void retrieveAll(){                 // 取出全部字符（置可读可写位到0）
        m_readIndex = 0;
        m_writeIndex = 0;
    }

    std::string retrieveAsString(){     // 以string形式取出
        std::string rsh(peek(), readableBytes());
        retrieveAll();
        return rsh;
    }

    std::string retrieveAsString(size_t len){
                assert(len <= (m_writeIndex-m_readIndex));
                std::string rsh(peek(), len);
                m_readIndex += len;
                if(m_readIndex == m_writeIndex)
                    retrieveAll();
                return rsh;
    }

    void append(const char* data, size_t len){      // 向缓冲区添加数据
        ensureWriteableSpace(len);
        std::copy(data, data+len, beginWrite());
        hasWritten(len);
    }

    void append(const std::string &rsh){
        append(rsh.data(), rsh.length());
    }

    void append(const Buffer& otherBuffer){         // 把其他缓冲区到数据添加进来
        append(otherBuffer.peek(), otherBuffer.readableBytes());
    }

    void ensureWriteableSpace(size_t len){      // 确保缓冲区有足够空间
        if(writableBytes() < len){
            makeBufferSpace(len);
        }
        assert(writableBytes() >= len);
    }

    void hasWritten(size_t len){            // 写入数据后移动m_writIndex
        m_writeIndex += len;
    }

    ssize_t readFd(QTcpSocket& fd, int *savedError);     // 从套接字读到缓冲区
    ssize_t writeFd(QTcpSocket fd, int *savedError);   // 从缓冲区写入套接字

    const char* findCRLF()const{
        const char CRLF[] = "\r\n";
        const char *crlf = std::search(peek(), beginWrite(), CRLF, CRLF+2);
        return crlf == beginWrite() ? nullptr : crlf;
    }

    const char* findCRLF(const char* start) const{
        assert(peek() <= start);
        assert(start <= beginWrite());
        const char CRLF[] = "\r\n";
        const char *crlf = std::search(start, beginWrite(), CRLF, CRLF+2);
        return crlf == beginWrite() ? nullptr : crlf;
    }

private:
    char* getBufferBegin(){             // 返回缓冲区头指针
        return &*m_buffer.begin();
    }
    const char* getBufferBegin() const{ // 返回缓冲区头指针
        return &*m_buffer.begin();
    }
    void makeBufferSpace(size_t len){   // 确保缓冲区有足够空间, 空间不够则扩容，空间够则将空闲区填满
        if(writableBytes() + prependableBytes() <len){
            m_buffer.resize(m_writeIndex + len);
        }else{
            size_t readable = readableBytes();
            std::copy(getBufferBegin() + m_readIndex,
                      getBufferBegin() + m_writeIndex,
                      getBufferBegin());
            m_readIndex = 0;
            m_writeIndex = m_readIndex + readable;
            assert(readable == readableBytes());
        }
    }
};

#endif // BUFFER_H
