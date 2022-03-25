//
// Created by zhou on 2022/3/11.
//

#include "LRUCache.h"
//
LRUCache::LRUCache() {
    m_head = new node(-1, nullptr);
    m_tail = new node(-1, nullptr);
    m_head->next = m_tail;
    m_tail->pre = m_head;
    live_Time = 500;
}

LRUCache::LRUCache(int liveTime) {
    m_head = new node(-1, nullptr);
    m_tail = new node(-1, nullptr);
    m_head->next = m_tail;
    m_tail->pre = m_head;
    live_Time = liveTime;
//    MyTime::getCurrentFormatTimeStr();
}

void LRUCache::del(node* ptr){
    ptr->pre->next = ptr->next;
    ptr->next->pre = ptr->pre;
}

void LRUCache::insert(node* ptr){
    ptr->next = m_head->next;
    ptr->pre = m_head;
    m_head->next->pre = ptr;
    m_head->next = ptr;
}

Connection *LRUCache::get(int key) {
    auto iter = m_map.find(key);
    if(iter != m_map.end()){

        node* ptr = m_map[key];

        // 从双向链表中移出
        del(ptr);
        // 插入到双向链表的首位
        insert(ptr);
        updateTime(ptr, MyTime::getTimeStampSecond());
        checkList();
        return m_map[key]->value;
    }
    checkList();
    return nullptr;
}

void LRUCache::put(int key, Connection *value) {
    auto iter = m_map.find(key);
    if(iter != m_map.end()){
        if(is_debug)
            std::cout << "更新结点cnn指针" << std::endl;

        remove(key);
        node* ptr = new node(key, value);
        insert(ptr);
        updateTime(ptr, MyTime::getTimeStampSecond());
        m_map.insert({key, ptr});
//        // 存在于缓存中
//        node* ptr = m_map[key];
//
//        ptr->value = value;
//        // 从双向链表中移出
//        del(ptr);
//        // 插入到双向链表的首位
//        insert(ptr);
//        updateTime(ptr, MyTime::getTimeStampSecond());
    }else{
        node* ptr = new node(key, value);
        insert(ptr);
        updateTime(ptr, MyTime::getTimeStampSecond());
        m_map.insert({key, ptr});
    }
    checkList();
}

LRUCache::~LRUCache() {
    node* ptr = m_head;
    while(ptr){
        node* tmp = ptr->next;
        delete ptr;
        ptr = tmp;
    }
    ptr = nullptr;
}

void LRUCache::updateTime(node* ptr, long curTime) {
    ptr->last_use_time = curTime;
}

void LRUCache::checkList() {
    if(is_debug)
        std::cout << "[LRUCache::checkList]" << std::endl;
    // 从链表尾部开始检查，删掉超时的节点
    if(m_tail->pre == m_head)
        return;
    node* ptr = m_tail->pre;
    long curTime = MyTime::getTimeStampSecond();
    while(ptr != m_head && curTime - ptr->last_use_time > live_Time){
        int key = ptr->key;
        remove(key);
        ptr = m_tail->pre;
    }
}

const std::unordered_map<int, node *> &LRUCache::getMap() {
    return m_map;
}

int LRUCache::size() {
    return m_map.size();
}

void LRUCache::remove(int key) {
    auto iter = m_map.find(key);
    if(iter == m_map.end()){
        if(is_debug)
            std::cout << "[LRUCache::remove] 删除失败，哈希表中无此账户 = " << key << std::endl;
        return ;
    }
    node* ptr = iter->second;
    m_map.erase(iter);
    del(ptr);
    delete ptr;
    ptr = nullptr;
    if(is_debug)
        std::cout << "[LRUCache::checkList] 删除节点：account=" << key << std::endl;
}
