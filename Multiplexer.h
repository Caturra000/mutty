#ifndef __MULTIPLEXER_H__
#define __MULTIPLEXER_H__
#include <bits/stdc++.h>
#include <sys/epoll.h>
#include "Socket.h"
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
    Multiplexer(): _epollFd(epoll_create1(EPOLL_CLOEXEC)) {}
};

#endif