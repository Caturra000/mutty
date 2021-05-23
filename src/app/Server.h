#ifndef __MUTTY_SERVER_H__
#define __MUTTY_SERVER_H__
#include <bits/stdc++.h>
#include "utils/Pointer.h"
#include "utils/Exchanger.h"
#include "utils/Object.h"
#include "utils/TypeTraits.h"
#include "utils/Compat.h"
#include "utils/FastIo.h"
#include "core/Looper.h"
#include "core/AcceptHandler.h"
#include "core/ConnectionPool.h"
#include "core/TcpHandler.h"
#include "core/TcpPolicy.h"
namespace mutty {

class Server {
public:
    void start();

    TCP_POLICY_CALLBACK_DEFINE(onConnect, _connectPolicy)
    TCP_POLICY_CALLBACK_DEFINE(onMessage, _messagePolicy)
    TCP_POLICY_CALLBACK_DEFINE(onWriteComplete, _writeCompletePolicy)
    TCP_POLICY_CALLBACK_DEFINE(onClose, _closePolicy)

    Server(Looper *looper, InetAddress localAddress)
        : _looper(looper),
          _acceptor(looper, localAddress) {}
private:
    void tcpCallbackInit(TcpHandler*);

    std::unique_ptr<TcpPolicy> _connectPolicy;
    std::unique_ptr<TcpPolicy> _messagePolicy;
    std::unique_ptr<TcpPolicy> _writeCompletePolicy;
    std::unique_ptr<TcpPolicy> _closePolicy;

    Pointer<Looper> _looper;
    AcceptHandler _acceptor;
    ConnectionPool _connections; 
};

inline void Server::start() {
    _acceptor.onNewConnection([this](std::weak_ptr<AcceptContext> context) {
        if(auto ctx = context.lock()) {
            auto connectionInfo = std::move(cast<
                std::pair<Socket, InetAddress>&>(ctx->exchanger));
            Socket &connectionSocket = connectionInfo.first;
            InetAddress &peerAddress = connectionInfo.second;
            auto &connection = _connections.createNewConnection(
                std::move(connectionSocket), ctx->localAddress, peerAddress);
            tcpCallbackInit(connection.get());
            connection->init();
        }
    });
    _acceptor.init();
}

inline void Server::tcpCallbackInit(TcpHandler *connection) {
    if(_connectPolicy) _connectPolicy->onConnect(connection);
    if(_messagePolicy) _messagePolicy->onMessage(connection);
    if(_writeCompletePolicy) _writeCompletePolicy->onWriteComplete(connection);
    if(_closePolicy) _closePolicy->onClose(connection);
}

} // mutty
#endif