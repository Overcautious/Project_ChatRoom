//
// Created by zy on 2022/2/27.
//

#include "DataEncoder.h"

    void DataEncoder::encodeElement(unsigned int data, unsigned int len) {
        char *c = hp + len - 1;
        for (int i = len; i > 0; --i) {
            *c = (char) (data & 0xff);
            --c;
            data >>= 8;
        }
        hp += len;
    }

    DataEncoder::DataEncoder() {

    }

    std::string
    DataEncoder::encode(unsigned int protocolId, unsigned int account, unsigned int dataType, unsigned int dataLength) {
        hp = head;
        encodeElement(protocolId, SIZE_PROTOCOL);
        encodeElement(account, SIZE_ACCOUNT);
        encodeElement(dataType, SIZE_TYPE);
        encodeElement(dataLength, SIZE_DATA);

        return std::string(head, sizeof(head));
    }
