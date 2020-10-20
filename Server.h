#ifndef __SERVER_H__
#define __SERVER_H__
#include <bits/stdc++.h>
#include "utils/Pointer.h"
#include "utils/Exchanger.h"
#include "Looper.h"
#include "MessageQueue.h"
#include "AcceptHandler.h"
#include "ConnectionPool.h"
// 使用socket内部类的更明确的封装
class Server {

public:
    Server(Looper *looper, InetAddress localAddress)
        : _looper(looper),
          _acceptor(looper, localAddress) {
        _acceptor.onNewConnectionWithCtx([this](AcceptContext *ctx) {
            std::pair<Socket, InetAddress> connectionInfo = std::move(cast<
                std::pair<Socket, InetAddress>&>(ctx->exchanger));
            Socket &connectionSocket = connectionInfo.first;
            InetAddress &peerAddress = connectionInfo.second;
            auto &_connection = _connections.createNewConnection(
                std::move(connectionSocket), ctx->localAddress, peerAddress);
            // TODO connection->setter Server用于组合时如何给出一个setter？
        });
    }
private:
    Pointer<Looper> _looper; // under _looper controll    looper可对付多个Server，也可直接new一个来用，后者需要用shared_ptr/unique_ptr&
    AcceptHandler _acceptor;
    ConnectionPool _connections;
};
#endif