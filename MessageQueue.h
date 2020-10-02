#ifndef __MESSAGE_QUEUE_H__
#define __MESSAGE_QUEUE_H__
#include <bits/stdc++.h>
#include "Message.h"

// 一个用于MPSC场合的消息队列
class MessageQueue {
public:
    int size() const { return _mq.size(); }
    bool hasNext() const { return !_mq.empty(); }
    Message next(); // 不保证线程安全，由Looper控制
    void post(Message msg);
    MessageQueue() = default;
    ~MessageQueue() = default;
    // 需要定制move行为，不移动mutex
    MessageQueue(MessageQueue &&rhs): _mq(std::move(rhs._mq)) {}
    MessageQueue& operator=(MessageQueue &&rhs) {
        if(this == &rhs) return *this;
        _mq = std::move(rhs._mq);
        return *this;
    }

    // used only for @Looper
    std::mutex& queueLock() { return _mutex; }

private:
    std::queue<Message> _mq;
    std::mutex _mutex;
};

inline void MessageQueue::post(Message msg) {
    std::lock_guard<std::mutex> _{_mutex};
    _mq.push(msg); 
}

inline Message MessageQueue::next() {
    if(_mq.empty()) return {};
    auto msg = _mq.front();
    _mq.pop();
    return msg;
}
#endif