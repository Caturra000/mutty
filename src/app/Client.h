#ifndef __CLIENT_H__
#define __CLIENT_H__
#include <bits/stdc++.h>
#include "utils/Pointer.h"
#include "utils/Timestamp.h"
#include "utils/Callable.h"
#include "utils/Compat.h"
#include "utils/FastIo.h"
#include "core/AsyncLooperContainer.h"
#include "core/Looper.h"
#include "core/ConnectionPool.h"
#include "core/TcpHandler.h"
#include "core/TcpPolicy.h"
#include "core/Transaction.h"
#include "net/InetAddress.h"
#include "net/Socket.h"
namespace mutty {

class Client {
public:
    Client(Looper *looper, InetAddress serverAddress)
        : _looper(looper), _serverAddress(serverAddress) {}

    std::future<bool> start();
    void connect();
    // void disconnect();
    // void stop();

    void enableRetry() { _retry = true; }
    void disableRetry() { _retry = false; }
    bool isRetryEnabled() { return _retry; }

    // unsafe
    void send(const void *data, int length) { 
        _connection->send(data, length);
    }

    // for async
    void join() { while(!_hasEnabled || !_looper->isReadyToStop() || _looper->onStop()); }

    // 提供给外部使用
    template <typename ...Args>
    Transaction startTransaction(Args &&...args) {
        return Transaction(_looper.get(), std::forward<Args>(args)...);
    }

    TCP_POLICY_CALLBACK_DEFINE(onConnect, _connectPolicy)
    TCP_POLICY_CALLBACK_DEFINE(onMessage, _messagePolicy)
    TCP_POLICY_CALLBACK_DEFINE(onWriteComplete, _writeCompletePolicy)
    TCP_POLICY_CALLBACK_DEFINE(onClose, _closePolicy)

private:
    void connecting(Socket socket/*, InetAddress address*/);
    void retry();
    void tcpCallbackInit(TcpHandler *connection);

private:
    Pointer<Looper> _looper;
    std::unique_ptr<TcpHandler> _connection;

    InetAddress _serverAddress;
    Millisecond _retryInterval {50ms};
    bool _retry {true};
    bool _hasEnabled {false};
    std::promise<bool> _connectedPromise;

// for TcpHandler

    std::unique_ptr<TcpPolicy> _connectPolicy;
    std::unique_ptr<TcpPolicy> _messagePolicy;
    std::unique_ptr<TcpPolicy> _writeCompletePolicy;
    std::unique_ptr<TcpPolicy> _closePolicy;

};



inline std::future<bool> Client::start() {
    auto scheduler = _looper->getScheduler();
    scheduler->runAt(now())
        .with([this] { connect(); });
    return _connectedPromise.get_future();
}

void Client::connect() {
    Socket socket;
    socket.setBlock(); // for connecting convenience
    int ret = socket.connect(_serverAddress);
    int err = (ret == 0) ? 0 : errno;
    switch (err) {
        case 0:
            connecting(std::move(socket));
        break;
        case EINPROGRESS:
        case EINTR:
        case EISCONN:
            // if nonblock, connecting here
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
    socket.setNonBlock();
    _connection = cpp11::make_unique<TcpHandler>(
        _looper.get(), std::move(socket), InetAddress{/*NONE*/}, _serverAddress);
    tcpCallbackInit(_connection.get()); // FIXME EINPROGRESS EINTR
    _connection->init();
    _hasEnabled = true;
    _connectedPromise.set_value(true);
}

inline void Client::retry() {
    auto scheduler = _looper->getScheduler();
    scheduler->runAfter(_retryInterval).with([this] { connect(); });
    // update 
    constexpr static Millisecond MAX_RETRY = 10s;
    Millisecond nextRetry = _retryInterval*2; 
    _retryInterval = std::min(MAX_RETRY, nextRetry);
}

inline void Client::tcpCallbackInit(TcpHandler *connection) {
    if(_connectPolicy) _connectPolicy->onConnect(connection);
    if(_messagePolicy) _messagePolicy->onMessage(connection);
    if(_writeCompletePolicy) _writeCompletePolicy->onWriteComplete(connection);
    if(_closePolicy) _closePolicy->onClose(connection);
}

} // mutty
#endif