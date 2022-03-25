//
// Created by zy on 2022/2/28.
//

#ifndef CHATSERVER_MYSQLCONNECTOR_H
#define CHATSERVER_MYSQLCONNECTOR_H
#include "all.h"
#include "MyTime.h"

    // MySQL的连接配置信息
#define DB_HOST "localhost"
#define DB_USER "root"
#define DB_PASSWORD "123"
#define DB_DATABASE "ChatUsers"
#define DB_PORT 3306

class MySQLConnector {
private:
    MYSQL *conn;

    void init();

    static std::shared_ptr<MySQLConnector> connector;   // 实例指针，可能会有很多类来使用该接口，因此使用shared_ptr
public:
    static std::shared_ptr<MySQLConnector> GetMySQLConnector();

    MySQLConnector();
    ~MySQLConnector();
    // 新建user信息
    bool insertUser(const std::string &account, const std::string &username, const std::string &password);

    // 查询是否存在该account
    bool queryUser(const std::string account);

    // 查询账户和密码
    std::pair<int, std::string> queryUser(const std::string &account, const std::string &password);
};


#endif //CHATSERVER_MYSQLCONNECTOR_H
