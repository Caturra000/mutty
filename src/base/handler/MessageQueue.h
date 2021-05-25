#ifndef __MUTTY_MESSAGE_QUEUE_H__
#define __MUTTY_MESSAGE_QUEUE_H__
#include <bits/stdc++.h>
#include "utils/NonCopyable.h"
#include "Message.h"
namespace mutty {

class MessageQueue: private NonCopyable {
public:
    int size() const { return _queue.size(); }
    bool hasNext() const { return !_queue.empty(); }
    Message next();
    void post(Message msg);
    MessageQueue() = default;
    ~MessageQueue() = default;
    MessageQueue(MessageQueue &&rhs): _queue(std::move(rhs._queue)) {}
    MessageQueue& operator=(MessageQueue &&rhs);
    std::mutex& lock() { return _mutex; }
    bool hasNextUnlock();

private:
    std::queue<Message> _queue;
    std::mutex _mutex;
};

inline Message MessageQueue::next() {
    if(_queue.empty()) return {};
    auto msg = _queue.front();
    _queue.pop();
    return msg;
}

inline void MessageQueue::post(Message msg) {
    std::lock_guard<std::mutex> _{_mutex};
    _queue.push(msg);
}

inline MessageQueue& MessageQueue::operator=(MessageQueue &&rhs) {
    if(this == &rhs) return *this;
    _queue = std::move(rhs._queue);
    return *this;
}

inline bool MessageQueue::hasNextUnlock() {
    std::lock_guard<std::mutex> _{_mutex};
    return !_queue.empty();
}

} // mutty
#endif