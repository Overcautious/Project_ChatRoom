//
// Created by zy on 2022/1/9.
//

#ifndef CHATSERVER_CONFIG_H
#define CHATSERVER_CONFIG_H

#define HOST "127.0.0.1"
#define PORT 8080
#define MAX_CONNECTIONS 1024

#define MAX_EVENTS 1024

#define HEAD_BEGIN 0x02  // 报头开始字符
#define HEAD_END 0x03    // 报头结束字符

#define SIZE_BUFFER_HEAD 8    // 报头总大小
#define SIZE_PROTOCOL 1  // 报头请求方式
#define SIZE_ACCOUNT 2   // 账号名
#define SIZE_TYPE 1      // 发送信息格式
#define SIZE_DATA 4      // 后序数据大小
// 请求方式
#define LOGIN 1
#define SEND 2
#define READ 3
#define NOTICE 4
#define ACK 5
#define LOGOUT 6
#define REGISTER 7
#define ONLINELIST 8
#define CLOSE 9
#define RECONNECT 10
// 发送格式
#define TEXT 1
#define IMAGE 2
//
#define LOGIN_SUCCESS 0
#define LOGIN_FAIL 1
#define LOGIN_EXIST 2

#define RECONNECT_SUCCESS 0
#define RECONNECT_FAIL 1

#define REGISTER_SUCCESS 0
#define REGISTER_FAIL 1



#endif //CHATSERVER_CONFIG_H
