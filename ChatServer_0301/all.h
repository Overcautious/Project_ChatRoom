//
// Created by zy on 2022/1/9.
//

#ifndef CHATSERVER_ALL_H
#define CHATSERVER_ALL_H
#include <map>
#include <set>
#include <list>
#include <mutex>
#include <queue>
#include <vector>
#include <thread>
#include <string>
#include <chrono>
#include <cstring>      // bzero
#include <cassert>
#include <iostream>
#include <memory>
#include <stdio.h>      // perror
#include <fcntl.h>      // fcntl
#include <unistd.h>     // fcntl, close
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>     // write
#include <sys/uio.h>    // readv
#include <iostream>
#include <algorithm>
#include <exception>
#include <functional>
#include <unordered_map>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <mysql/mysql.h>
#include <sys/socket.h>     // socket, setsockopt, bind, listen
#include <arpa/inet.h>      // htonl, htons
#include <condition_variable>
#include <json/json.h>
#include "config.h"

#define gettid() syscall(__NR_gettid)

extern bool is_debug;

#endif //CHATSERVER_ALL_H
