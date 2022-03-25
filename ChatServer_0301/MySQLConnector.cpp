//
// Created by zy on 2022/2/28.
//

#include "MySQLConnector.h"

std::shared_ptr<MySQLConnector> MySQLConnector::connector = nullptr;

void MySQLConnector::init() {
    conn = mysql_init(NULL);
    conn = mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASSWORD, DB_DATABASE, DB_PORT, NULL, 0);
    if (!conn) {
        std::cout << "mysql_real_connect fail" << std::endl;
        exit(-1);   // 连接失败，退出程序
    }
    std::cout << "mysql_real_connect success" << std::endl;
    mysql_query(conn, "set name utf8");
}

MySQLConnector::MySQLConnector() {

}



bool
MySQLConnector::insertUser(const std::string &account, const std::string &username, const std::string &password) {
    std::string querySql = std::string("insert into user values(") + account + ",\"" +
                           username + "\",\"" + password + "\",\"" + MyTime::getCurrentFormatTimeStr() + "\")";
    int res = mysql_query(conn, querySql.data());   // 操作成功会返回0，
    return res == 0;
}

bool MySQLConnector::queryUser(const std::string account) {
    std::string querySql = std::string("select account from user where account = ") + account;
    int res = mysql_query(conn, querySql.data());
    bool flag = false;
    if (!res) {
        MYSQL_RES *result = mysql_store_result(conn);
        if (result && mysql_num_rows(result)) {   // 结果是否为空
            flag = true;
        }
        mysql_free_result(result);
    }
    return flag;
}

std::pair<int, std::string> MySQLConnector::queryUser(const std::string &account, const std::string &password) {
    std::pair<int, std::string> ans;
    std::string querySql =
            std::string("select account, username from user where account = ") + account + " and password = \"" +
            password + "\"";
    int res = mysql_query(conn, querySql.data());
    if (!res) {
        MYSQL_RES *result = mysql_store_result(conn);
        if (result && mysql_num_rows(result)) {   // 结果是否为空
            MYSQL_ROW row = mysql_fetch_row(result);
            ans.first = atoi(row[0]);
            ans.second = row[1];
        }
        mysql_free_result(result);
    }
    return ans;
}

    MySQLConnector::~MySQLConnector() {
        std::cout << "close mysql" << std::endl;
        mysql_close(conn);

    }

std::shared_ptr<MySQLConnector> MySQLConnector::GetMySQLConnector() {
    if (connector == nullptr) {
        connector = std::shared_ptr<MySQLConnector>(new MySQLConnector());
        connector->init();
    }
    return connector;
}
