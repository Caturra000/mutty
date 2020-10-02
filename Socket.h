#ifndef __SOCKET_H__
#define __SOCKET_H__


#include <bits/stdc++.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "utils/Noncopyable.h"
#include "InetAddress.h"
#include "Unsafe.h"
#include "netinet/tcp.h"



// 针对socket fd的浅层封装，要求只有fd一个成员
struct Socket: Noncopyable {
public:
    
    Socket accept(InetAddress &clientAddress);
    Socket accept();
    void finish() {}

    // bool isValid() { return _socketFd > 0; }
    void detach() {  } 
    
    Socket(): _socketFd(socket(AF_INET, SOCK_STREAM, 0)) { if(_socketFd < 0) throw std::exception(); }
    ~Socket() { close(_socketFd); }
    
    
    struct Option {
        const static int NO_DELAY   = 1 << 0;
        const static int REUSE_ADDR = 1 << 1;
        const static int REUSE_PORT = 1 << 2;
        const static int KEEP_ALIVE = 1 << 3;

        const static int DEFAULT      = 0;
        const static int FULL_FEATURE = std::numeric_limits<int>::max(); // (1LL << 31) - 1; 

        const int _option;

        Option(int option = DEFAULT): _option(option) {}
    };



    
    void asServer(const InetAddress &serverAddress, int backlog = 128, Option option = Option::FULL_FEATURE);

protected:
    Socket(int socketFd): _socketFd(socketFd) { } // unsafe

public:
    const int _socketFd;

private:

    // IMPROVEMENT: 把error-code改为try-catch，error-code的存在让封装变得麻烦
    // TODO: exception类型需要定义

    // wrapper
    void bind(const InetAddress &address) { if(::bind(_socketFd, (const sockaddr*)(&address), sizeof(InetAddress))) throw std::exception(); }
    void listen(int backlog = 128) { if(::listen(_socketFd, backlog)) throw std::exception(); }
    void bindAndListen(const InetAddress &serverAddress, int backlog) { bind(serverAddress); listen(backlog); } // true if error      // 这里的bool是个败笔
    // setter
    void setNoDelay(bool on = true);
    void setReuseAddr(bool on = true);
    void setReusePort(bool on = true);
    void setKeepAlive(bool on = true);

    // 更多的tcp/socket选项还是给unix api来做吧

};




inline Socket Socket::accept(InetAddress &address) {
    int connectFd = ::accept4(_socketFd, (sockaddr*)(&address), nullptr,
                        SOCK_NONBLOCK | SOCK_CLOEXEC);
    return Socket(connectFd);
}

inline Socket Socket::accept() {
    int connectFd = ::accept4(_socketFd, nullptr, nullptr,
                        SOCK_NONBLOCK | SOCK_CLOEXEC);
    return Socket(connectFd);
}


inline void Socket::asServer(const InetAddress &serverAddress, int backlog, Option option) {
    bindAndListen(serverAddress, backlog);
    int opt = option._option;
    // 固定的配置，这里就不写的oop了
    if(opt & Option::NO_DELAY) setNoDelay();
    if(opt & Option::REUSE_ADDR) setReuseAddr();
    if(opt & Option::REUSE_PORT) setReusePort();
    if(opt & Option::KEEP_ALIVE) setKeepAlive();
}

inline void Socket::setNoDelay(bool on) {
    int optval = on;
    if(setsockopt(_socketFd, IPPROTO_TCP, TCP_NODELAY, &optval,
            static_cast<socklen_t>(sizeof optval))) {
        throw std::exception();
    }
}

inline void Socket::setReuseAddr(bool on) {
    int optval = on;
    if(setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, &optval,
            static_cast<socklen_t>(sizeof optval))) {
        throw std::exception();
    }
}


inline void Socket::setReusePort(bool on) {
    int optval = on;
    if(setsockopt(_socketFd, SOL_SOCKET, SO_REUSEPORT, &optval,
            static_cast<socklen_t>(sizeof optval))) {
        throw std::exception();
    }
}

inline void Socket::setKeepAlive(bool on) {
    int optval = on;
    if(setsockopt(_socketFd, SOL_SOCKET, SO_KEEPALIVE, &optval,
            static_cast<socklen_t>(sizeof optval))) {
        throw std::exception();
    }
}






#endif