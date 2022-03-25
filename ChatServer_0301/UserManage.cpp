//
// Created by zy on 2022/3/1.
//

#include "UserManage.h"

bool UserManage::checkAccount(const std::string &account) {
    // 账户长度为3-6位，且都为数字，且不超过65535（2位存储，不超过2的16次方）
    if(account.size() < 3 || account.size() > 6){
        printf("账户长度不满足条件\n");
        return false;
    }
    for(const char rhs : account){
        if(rhs < '0' || rhs > '9'){
            printf("账号只能是数字\n");
            return false;
        }
    }
    if(std::atoi(account.data()) > 65535){
        printf("账号不能超过65535\n");
        return false;
    }
    return true;
}

    bool UserManage::checkPassword(const std::string &password) {
        // 密码长度为6-16位，且由字母，数字组成
        if(password.size() < 6 || password.size() > 16){
            printf("密码长度不符合要求\n");
            return false;
        }
        for(char rhs : password){
            if( ! ((rhs >= 'a' && rhs <= 'z') || (rhs >= '0' && rhs <='9') || (rhs >= 'A' && rhs <= 'Z'))){
                printf("密码只能由字母，数字组成\n");
                return false;
            }
        }
        return true;
    }

    UserManage::UserManage() {
        connector = MySQLConnector::GetMySQLConnector();
    }

    std::pair<int, std::string> UserManage::checkLogin(const std::string &account, const std::string &password) {\
        std::pair<int, std::string> userInfo;
        if(checkAccount(account) && checkPassword(password)){
            userInfo = connector->queryUser(account, password);
        }
        return userInfo;
    }

    bool
    UserManage::registerUser(const std::string &account, const std::string &username, const std::string &password) {
        return checkAccount(account) && checkPassword(password) && connector->insertUser(account, username, password);
    }

    bool UserManage::isRegistered(const std::string &account) {
        return checkAccount(account) && connector->queryUser(account);
    }

