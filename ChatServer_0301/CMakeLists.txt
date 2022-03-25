cmake_minimum_required(VERSION 3.10)
project(ChatServer)

set(CMAKE_CXX_STANDARD 11)

set(INC_DIR /usr/include/mysql/)
set(lib_DIR /usr/lib/x84_64-linux-gnu)
include_directories(${INC_DIR})
link_directories(${lib_DIR})
link_libraries(mysqlclient)


add_executable(ChatServer main.cpp
        Server.cpp Server.h all.h config.h Epoll.cpp Epoll.h Connection.cpp Connection.h Buffer.cpp Buffer.h ThreadPool.cpp ThreadPool.h
        DataEncoder.cpp DataEncoder.h MyTime.h MyTime.cpp MySQLConnector.cpp MySQLConnector.h UserManage.cpp UserManage.h LRUCache.cpp LRUCache.h)

target_link_libraries(ChatServer libjsoncpp.a)
target_link_libraries(ChatServer pthread)
