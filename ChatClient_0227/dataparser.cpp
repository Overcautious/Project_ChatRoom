#include "dataparser.h"

DataParser::DataParser(char * buffer)
{
    bp = buffer;
}
DataParser::DataParser(string dp)
{
    m_dp = dp;
    bp = (char*)dp.c_str();
}

void DataParser:: baseParse() {
    this->protocolId = parseInt(SIZE_PROTOCOL);
    this->account = parseInt(SIZE_ACCOUNT);
    this->dataType = parseInt(SIZE_TYPE);
    this->dataLength = parseInt(SIZE_DATA);
}

unsigned int DataParser::parseInt(int len) {
    unsigned int sum = 0;
    unsigned int i = 0;
    for (char* end = bp + len - 1; bp <= end; end--) {
        sum = sum + (((unsigned int)((unsigned char)(*end))) << i);
        i += 8;
    }
    bp = bp + len;
    return sum;
}

unsigned int DataParser::getProtocolId() {
    return this->protocolId;
}
unsigned int DataParser::getAccount() {
    return this->account;
}
unsigned int DataParser::getDataType() {
    return this->dataType;
}
unsigned int DataParser::getDataLength() {
    return this->dataLength;
}

DataParser::~DataParser() {

}
