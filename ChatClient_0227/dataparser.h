#ifndef DATAPARSER_H
#define DATAPARSER_H
#include "protocolmsg.h"
#include <string>
using namespace std;

class DataParser
{
private:

    char* bp;
    string m_dp;
    unsigned int protocolId;
    unsigned int account;
    unsigned int dataType;
    unsigned int dataLength;

    unsigned int parseInt(int len);

public:
    DataParser(char* buffer);
    DataParser(string dp);

    void baseParse();

    unsigned int getProtocolId();

    unsigned int getAccount();

    unsigned int getDataType();

    unsigned int getDataLength();

    ~DataParser();
};

#endif // DATAPARSER_H
