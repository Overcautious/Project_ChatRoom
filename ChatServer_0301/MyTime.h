//
// Created by zy on 2022/2/28.
//

#ifndef CHATSERVER_MYTIME_H
#define CHATSERVER_MYTIME_H
#include "all.h"

class MyTime {
public:
    static std::string getCurrentFormatTimeStr();

    static std::string getTimeStampStr();
    static long getTimeStampSecond();
};



#endif //CHATSERVER_MYTIME_H