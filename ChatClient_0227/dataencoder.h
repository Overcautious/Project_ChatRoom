#ifndef DATAENCODER_H
#define DATAENCODER_H
#include "protocolmsg.h"
#include <string>
#include <QString>
using namespace std;

class DataEncoder
{
private:
    char head[SIZE_BUFFER_HEAD];
    char * hp;
    void encodeElement(unsigned int data, unsigned int len);

public:
    DataEncoder();
    string encode(unsigned int protocolId, unsigned int account, unsigned int dataType, unsigned int dataLength);
};

#endif // DATAENCODER_H
