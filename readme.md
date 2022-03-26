### 基于C++11开发的聊天室
#### 介绍
使用 C++实现的 ubuntu 环境下的聊天小项目，采用 C/S 架构，支持注册、登录、记录登录状态、群聊、心跳包检测功能。

#### 主要功能
1. 用户注册、登录、登录账户是否已经登录。
2. 心跳包检测机制，用于客户端判断自身是否掉线，以及服务端检测客户端在线状态。
3. LRU机制，用来服务端管理在线账户。
4. 可变参数线程池，动态数量的线程池，以满足不同请求量。
5. 数据报通信协议，防止数据粘包。
#### 服务端涉及到的技术点
* epoll多路IO转接机制
* 常用STL(vector, unordered_map, queue)
* 锁``mutex``，信号量``condition_variable``
* 智能指针(shared_ptr, unique_ptr)
* 使用std::function + std::bind绑定的方式实现回调函数
* 可变参数线程池(thread)
* jsoncpp解析json数据
* MySQL基本操作

#### 客户端涉及到的技术点
* 常用QT控件(QWidget, QListWidget, QLabel, QPushButton)
* QT信号与槽
* QJsonObject完成json数据的解析
* QTcpSocket连接服务器


---
### 未来的工作：
 -[ ] 实现用户私聊，用户单独建立群聊
 -[ ] 线程池能够接受任何类型带参数的任务函数

 ... 