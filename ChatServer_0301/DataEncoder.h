//
// Created by zy on 2022/2/27.
//

#ifndef CHATSERVER_DATAENCODER_H
#define CHATSERVER_DATAENCODER_H
#include "config.h"
#include "all.h"

    class DataEncoder {
    private:
        char head[SIZE_BUFFER_HEAD];
        char *hp;

    public:
        DataEncoder();
        void encodeElement(unsigned int data, unsigned int len);
        std::string encode(unsigned int protocolId, unsigned int account, unsigned int dataType, unsigned int dataLength);
    };


#endif //CHATSERVER_DATAENCODER_H
