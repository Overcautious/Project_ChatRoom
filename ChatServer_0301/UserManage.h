//
// Created by zy on 2022/3/1.
//

#ifndef CHATSERVER_USERMANAGE_H
#define CHATSERVER_USERMANAGE_H
#include "all.h"
#include "MySQLConnector.h"


class UserManage {
private:
    std::shared_ptr<MySQLConnector> connector;
    bool checkAccount(const std::string& account);  // 验证账户格式
    bool checkPassword(const std::string& password);    // 验证密码格式
public:
    UserManage();

    std::pair<int, std::string> checkLogin(const std::string& account, const std::string& password); // 验证密码
    bool registerUser(const std::string& account, const std::string& username , const std::string& password);   // 注册
    bool isRegistered(const std::string& account);   // 查验数据库中是否存在该账户
};



#endif //CHATSERVER_USERMANAGE_H
