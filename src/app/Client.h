#ifndef __CLIENT_H__
#define __CLIENT_H__
#include <bits/stdc++.h>
#include "utils/Pointer.h"
#include "utils/Timestamp.h"
#include "utils/Callable.h"
#include "utils/Compat.h"
#include "core/Looper.h"
#include "core/ConnectionPool.h"
#include "core/TcpHandler.h"
#include "net/InetAddress.h"
#include "net/Socket.h"

class Client {
public:
    Client(Looper *looper, InetAddress serverAddress)
        : _looper(looper), _serverAddress(serverAddress) {}

    void connect();
    void connecting(Socket socket/*, InetAddress address*/);
    void retry();
    // void disconnect();
    // void stop();

    void enableRetry() { _retry = true; }
    void disableRetry() { _retry = false; }
    bool isRetryEnabled() { return _retry; }

private:
    void tcpCallbackInit(TcpHandler *connection); // TODO

private:
    Pointer<Looper> _looper;
    std::unique_ptr<TcpHandler> _connection;

    InetAddress _serverAddress;
    Millisecond _retryInterval {50ms};
    bool _retry {true};
    // SingleConnectionPool _reusableConnection;
    // 目前考虑的问题是Handler是否要求无状态 
    // ctx并不需维护什么状态，只需有TcpHandler/TcpContext就好了

};

void Client::connect() {
    Socket socket;
    int ret = socket.connect(_serverAddress);
    int err = (ret == 0) ? 0 : errno;
    switch (err) {
        case 0:
        case EINPROGRESS:
        case EINTR:
        case EISCONN:
            connecting(std::move(socket));
        break;
        case EAGAIN:
        case EADDRINUSE:
        case EADDRNOTAVAIL:
        case ECONNREFUSED:
        case ENETUNREACH:
            if(isRetryEnabled()) retry();
        break;
        default:
            // socket安全析构
        break;
    }
}

inline void Client::connecting(Socket socket) {
    _connection = cpp11::make_unique<TcpHandler>(
        _looper.get(), std::move(socket), InetAddress{/*NONE*/}, _serverAddress);
    // TODO callback
}

inline void Client::retry() {
    auto timer = _looper->getTimer();
    timer->runAfter(_retryInterval).with([this] { connect(); });
    // update 
    constexpr static Millisecond MAX_RETRY = 10s;
    Millisecond nextRetry = _retryInterval*2; 
    _retryInterval = std::min(MAX_RETRY, nextRetry);
}
#endif