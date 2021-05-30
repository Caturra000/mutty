#ifndef __MUTTY_SERVER_H__
#define __MUTTY_SERVER_H__
#include <bits/stdc++.h>
#include "utils/Pointer.h"
#include "utils/Exchanger.h"
#include "utils/Object.h"
#include "utils/TypeTraits.h"
#include "utils/Compat.h"
#include "utils/FastIo.h"
#include "utils/NonCopyable.h"
#include "core/Looper.h"
#include "core/AcceptBridge.h"
#include "core/ConnectionPool.h"
#include "core/TcpBridge.h"
#include "core/TcpPolicy.h"
#include "log/Log.h"
namespace mutty {

class Server: private NonCopyable {
public:
    void start();
    void stop(); // cannot restart, should be protected by _looper if called in context (unsafe)

    TCP_POLICY_CALLBACK_DEFINE(onConnect, _connectPolicy)
    TCP_POLICY_CALLBACK_DEFINE(onMessage, _messagePolicy)
    TCP_POLICY_CALLBACK_DEFINE(onWriteComplete, _writeCompletePolicy)
    TCP_POLICY_CALLBACK_DEFINE(onClose, _closePolicy)

    Server(Looper *looper, InetAddress localAddress)
        : _looper(looper),
          _acceptor(std::make_shared<AcceptContext>(looper, localAddress)) {}
    ~Server();
private:
    void tcpCallbackInit(TcpContext*);

    std::unique_ptr<TcpPolicy> _connectPolicy;
    std::unique_ptr<TcpPolicy> _messagePolicy;
    std::unique_ptr<TcpPolicy> _writeCompletePolicy;
    std::unique_ptr<TcpPolicy> _closePolicy;

    Pointer<Looper> _looper;
    std::shared_ptr<AcceptContext> _acceptor;
    ConnectionPool _connections;
    bool _start = false;
    bool _stop = false; // TODO use atomic
};

inline void Server::start() {
    if(_start) return;
    _acceptor->onNewConnection([this](AcceptContext *context) {
        // auto connectionInfo = std::move(cast<
        //     std::pair<Socket, InetAddress>&>(context->exchanger));
        auto connectionInfo = context->poll();
        if(!connectionInfo) return;
        Socket &connectionSocket = connectionInfo->first;
        InetAddress &peerAddress = connectionInfo->second;
        auto connection = _connections.createNewConnection(
            std::move(connectionSocket), context->localAddress, peerAddress);
        tcpCallbackInit(connection.get());
        connection->start();
    });
    _acceptor->start();
    _start = true;
    MUTTY_LOG_INFO("server started.");
}

inline void Server::stop() {
    if(!_start) return;
    _acceptor->stop();
    _connections.stop();
    _stop = true;
}

inline Server::~Server() {
    if(_start && !_stop) {
        stop();
    }
}

inline void Server::tcpCallbackInit(TcpContext *connection) {
    if(_connectPolicy) _connectPolicy->onConnect(connection);
    if(_messagePolicy) _messagePolicy->onMessage(connection);
    if(_writeCompletePolicy) _writeCompletePolicy->onWriteComplete(connection);
    if(_closePolicy) _closePolicy->onClose(connection);
}

} // mutty
#endif