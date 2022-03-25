#include "dataencoder.h"
#include "dataparser.h"
#include <QDebug>



DataEncoder::DataEncoder() {

}

string DataEncoder::encode(unsigned int protocolId, unsigned int account, unsigned int dataType, unsigned int dataLength) {
    hp = head;
    qDebug() << "dead info: " << protocolId << account << dataType << dataLength;
    encodeElement(protocolId, SIZE_PROTOCOL);
    encodeElement(account, SIZE_ACCOUNT);
    encodeElement(dataType, SIZE_TYPE);
    encodeElement(dataLength, SIZE_DATA);
    DataParser dp(head);

//    unsigned int _protocolId = dp.getProtocolId();
//    unsigned int _account = dp.getAccount();
//    unsigned int _dataType = dp.getDataType();
//    unsigned int _dataLength = dp.getDataLength();
//    qDebug() << _protocolId << _account << _dataType << _dataLength;

    return string(head,sizeof(head));
}

void DataEncoder::encodeElement(unsigned int data, unsigned int len) {
    char* c = hp + len - 1;
    for (int i = len; i > 0; i--) {

        *c = (char)(data & 0xff);   // 每次取低八位 0xff：11111111
        c--;
        data >>= 8;
    }
    hp = hp + len;
}
