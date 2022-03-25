//
// Created by zhou on 2022/3/11.
//

#ifndef CHATSERVER_LRUCACHE_H
#define CHATSERVER_LRUCACHE_H
#include "all.h"
#include "MyTime.h"
class Connection;

class node{
public:
    node* pre, * next;
    int key;    // 对应 account，
    Connection* value;  // 对应链接
    long last_use_time;
    node():pre(nullptr), next(nullptr), key(0), value(nullptr), last_use_time(0){}
    node(node* p, node* n, int k , Connection* v):pre(p), next(n), value(v), key(k), last_use_time(0){}
    node(int k, Connection* v):key(k), value(v), pre(nullptr), next(nullptr), last_use_time(0){}
    node(int k, Connection* v, long t):key(k), value(v), last_use_time(t),pre(nullptr), next(nullptr){}
};

class LRUCache {
private:
    node* m_head, *m_tail;
    int live_Time; // 达到多久的时间就从链表中删除
    std::unordered_map<int, node*> m_map;
    void del(node* ptr);
    void insert(node* ptr);
public:
    LRUCache();
    LRUCache(int liveTime);
    ~LRUCache();
//        std::cout << " !!!!";
//    }

    Connection* get(int key);
    void put(int key, Connection* value);

    void updateTime(node* ptr, long curTime);
    void checkList();   // 从链表尾部开始检查，删掉超时的节点

    const std::unordered_map<int, node*>& getMap();
    int size();
    void remove(int key);

};


#endif //CHATSERVER_LRUCACHE_H
