#ifndef __MULTIPLEXER_H__
#define __MULTIPLEXER_H__
#include <bits/stdc++.h>
#include <sys/epoll.h>
#include "Socket.h"
#include "utils/Timestamp.h"
// selector poller之类的封装吧

// 负责epoll的封装
class Multiplexer {
private:
    // 多个连接的对象？
    std::vector<Socket> _sockets;
    std::vector<epoll_event> _events;
    int _epollFd;
    // _wakeupFd

public:
    Multiplexer(): _epollFd(epoll_create1(EPOLL_CLOEXEC)), _events(16) {
        if(_epollFd < 0) throw std::exception();
    }
    ~Multiplexer() { close(_epollFd); }

    std::vector<char> poll(Nanosecond timeout) { //支持ns的表示，虽然实现上用不着，因为epoll粒度是ms
        std::vector<char> res;
        int count = epoll_wait(_epollFd, _events.data(), _events.size(), 
                        std::chrono::duration_cast<Millisecond>(timeout).count());
        if(count >= 0) {
            // .....TODO......
            if(count == _events.size() && _events.size() < 0x7fffffff) { // FIXME: 这一段可读性太差，应该封装为独立的函数
                _events.resize(_events.size() << 1);
            }
        } else {
            // TODO: 对errno的处理
        }
        return res;
    }
};

#endif