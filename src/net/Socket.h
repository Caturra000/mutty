#ifndef __MUTTY_SOCKET_H__
#define __MUTTY_SOCKET_H__
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <bits/stdc++.h>
#include "utils/NonCopyable.h"
#include "throws/Exceptions.h"
#include "log/Log.h"
#include "InetAddress.h"
namespace mutty {

class Socket: private NonCopyable {
public:
    int fd() const { return _socketFd; }

    struct Option;
    void config(Option option);

    void bind(const InetAddress &address);
    void listen(int backlog = SOMAXCONN);
    Socket accept(InetAddress &clientAddress);
    Socket accept();
    int connect(const InetAddress &address);
    void detach() { _socketFd = INVALID_FD; }
    void shutdown() { ::shutdown(_socketFd, SHUT_WR); }

    void setNoDelay(bool on = true);
    void setReuseAddr(bool on = true);
    void setReusePort(bool on = true);
    void setKeepAlive(bool on = true);
    void setBlock();
    void setNonBlock();

    void swap(Socket &that) { std::swap(this->_socketFd, that._socketFd); }

    static constexpr int INVALID_FD = -1;

    // TODO remove
    struct Option {
        constexpr static int NO_DELAY   = 1 << 0;
        constexpr static int REUSE_ADDR = 1 << 1;
        constexpr static int REUSE_PORT = 1 << 2;
        constexpr static int KEEP_ALIVE = 1 << 3;

        constexpr static int DEFAULT      = 0;
        constexpr static int FULL_FEATURE = std::numeric_limits<int>::max(); // (1LL << 31) - 1; 

        const int _option;

        Option(int option = DEFAULT): _option(option) {}
    };

    Socket();
    ~Socket();
    explicit Socket(int socketFd): _socketFd(socketFd) { assert(_socketFd != INVALID_FD); }
    Socket(Socket &&rhs): _socketFd(rhs._socketFd) { rhs._socketFd = INVALID_FD; }
    Socket& operator=(Socket &&rhs);

    // int fd = std::move(socket);
    operator int() &&;

private:
    int _socketFd;
};

inline void Socket::bind(const InetAddress &address) {
    MUTTY_LOG_DEBUG("socket binding. fd =", _socketFd);
    if(::bind(_socketFd, (const sockaddr*)(&address), sizeof(InetAddress))) {
        MUTTY_LOG_WARN("socket bind failed. fd =", _socketFd,
            "throws exception.", "errorno = ", errno);
        throw SocketBindException(errno);
    }
}
inline void Socket::listen(int backlog) {
    MUTTY_LOG_DEBUG("socket listening. fd =", _socketFd);
    if(::listen(_socketFd, backlog)) {
         MUTTY_LOG_WARN("socket listen failed. fd =", _socketFd,
            "throws exception.", "errorno = ", errno);
        throw SocketListenException(errno);
    }
}


inline Socket Socket::accept(InetAddress &address) {
    socklen_t len = sizeof(address);
    int connectFd = ::accept4(_socketFd, (sockaddr*)(&address), &len,
                        SOCK_NONBLOCK | SOCK_CLOEXEC);
    if(connectFd < 0) {
        MUTTY_LOG_WARN("socket::accept failed, throws SocketAcceptException.", "errno =", errno);
        throw SocketAcceptException(errno);
    }
    MUTTY_LOG_DEBUG("socket accepted. connected fd =", connectFd, "listen fd =", _socketFd);
    return Socket(connectFd);
}

inline Socket Socket::accept() {
    int connectFd = ::accept4(_socketFd, nullptr, nullptr,
                        SOCK_NONBLOCK | SOCK_CLOEXEC);
    if(connectFd < 0) {
        MUTTY_LOG_WARN("socket::accept failed, throws SocketAcceptException.", "errno =", errno);
        throw SocketAcceptException(errno);
    }
    MUTTY_LOG_DEBUG("socket accepted. connected fd =", connectFd, "listen fd =", _socketFd);
    return Socket(connectFd);
}

inline int Socket::connect(const InetAddress &address) {
    return ::connect(_socketFd, (const sockaddr *)(&address), sizeof(address));
}

inline void Socket::config(Option option) {
    int opt = option._option;
    if(opt & Option::NO_DELAY) setNoDelay();
    if(opt & Option::REUSE_ADDR) setReuseAddr();
    if(opt & Option::REUSE_PORT) setReusePort();
    if(opt & Option::KEEP_ALIVE) setKeepAlive();
}

inline void Socket::setNoDelay(bool on) {
    int optval = on;
    if(::setsockopt(_socketFd, IPPROTO_TCP, TCP_NODELAY, &optval,
            static_cast<socklen_t>(sizeof optval))) {
        throw SocketException(errno);
    }
}

inline void Socket::setReuseAddr(bool on) {
    int optval = on;
    if(::setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, &optval,
            static_cast<socklen_t>(sizeof optval))) {
        throw SocketException(errno);
    }
}

inline void Socket::setReusePort(bool on) {
    int optval = on;
    if(::setsockopt(_socketFd, SOL_SOCKET, SO_REUSEPORT, &optval,
            static_cast<socklen_t>(sizeof optval))) {
        throw SocketException(errno);
    }
}

inline void Socket::setKeepAlive(bool on) {
    int optval = on;
    if(::setsockopt(_socketFd, SOL_SOCKET, SO_KEEPALIVE, &optval,
            static_cast<socklen_t>(sizeof optval))) {
        throw SocketException(errno);
    }
}


inline void Socket::setBlock() {
    int flags = ::fcntl(_socketFd, F_GETFL, 0);
    ::fcntl(_socketFd, F_SETFL, flags & (~SOCK_NONBLOCK));
}

inline void Socket::setNonBlock() {
    int flags = ::fcntl(_socketFd, F_GETFL, 0);
    ::fcntl(_socketFd, F_SETFL, flags | SOCK_NONBLOCK);
}

inline Socket::Socket()
    : _socketFd(::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0)) {
    if(_socketFd < 0) {
        MUTTY_LOG_WARN("creating socket but failed, throws exception.", "errno =", errno);
        throw SocketCreateException(errno);
    }
    MUTTY_LOG_DEBUG("socket created. fd =", _socketFd);
}

inline Socket::~Socket() {
    if(_socketFd != INVALID_FD) {
        ::close(_socketFd);
        MUTTY_LOG_DEBUG("socket closed. fd =", _socketFd);
    }
}

inline Socket& Socket::operator=(Socket &&rhs) {
    Socket(static_cast<Socket&&>(rhs)).swap(*this);
    return *this;
}

inline Socket::operator int() && {
    int fd = _socketFd;
    _socketFd = INVALID_FD;
    return fd;
}

} // mutty

#endif