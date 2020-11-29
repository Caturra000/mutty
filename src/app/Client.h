#ifndef __CLIENT_H__
#define __CLIENT_H__
#include <bits/stdc++.h>
#include "utils/Pointer.h"
#include "utils/Timestamp.h"
#include "core/Looper.h"
#include "core/ConnectionPool.h"
#include "core/TcpHandler.h"
#include "net/InetAddress.h"
#include "net/Socket.h"

class Client {
public:
    Client(Looper *looper, InetAddress serverAddress)
        : _looper(looper) {}

    void connect();
    void connecting(Socket socket);
    void retry();
    // void disconnect();
    // void stop();

    void enableRetry() { _retry = true; }
    bool isRetryEnabled() { return _retry; }

    // onNewConnection

private:
    Pointer<Looper> _looper;
    std::unique_ptr<TcpHandler> _connection;

    Millisecond _retryInterval {50ms};
    bool _retry {false};
    // SingleConnectionPool _reusableConnection;
    // 目前考虑的问题是Handler是否要求无状态 
    // ctx并不需维护什么状态，只需有TcpHandler/TcpContext就好了

};

void Client::connect() {
    Socket socket;
    // socket.connect(address); TODO
    int err;
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
            retry();
        break;
        default:
            // socket安全析构
        break;
    }
}

inline void Client::connecting(Socket socket) {
    _connection.reset(); // TODO
}

inline void Client::retry() {
    auto timer = _looper->getTimer();
    timer->runAfter(_retryInterval).with([]{}); // TODO
    // TODO update _retryInterval
}
#endif