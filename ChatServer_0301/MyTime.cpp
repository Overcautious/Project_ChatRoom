//
// Created by zy on 2022/2/28.
//
#include "MyTime.h"
// 获取当前时间戳
std::string MyTime::getTimeStampStr() {
    //return "123";
    timeval stamp{};
    gettimeofday(&stamp, NULL);
    return std::to_string(stamp.tv_sec) + std::to_string(stamp.tv_usec);
}

std::string MyTime::getCurrentFormatTimeStr() {
    time_t timep;
    time(&timep);
    char tmp[64] = {'\0'};
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));
    return std::string(tmp);
}

long MyTime::getTimeStampSecond(){
        timeval stamp{};
        gettimeofday(&stamp, NULL);
        return stamp.tv_sec;
}