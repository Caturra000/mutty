#ifndef __SERVER_H__
#define __SERVER_H__
#include <bits/stdc++.h>
#include "utils/Pointer.h"
#include "utils/Exchanger.h"
#include "utils/Object.h"
#include "Looper.h"
#include "MessageQueue.h"
#include "AcceptHandler.h"
#include "ConnectionPool.h"
#include "TcpHandler.h"

#define SERVER_CALLBACK_DEFINE(functionName, callbackMember, useCtxFlag) \
    template <typename Functor, typename ...Args> \
    void functionName(Functor &&functor, Args &&...args) { \
        callbackMember = LazyEvaluate::lazy(std::forward<Functor>(functor), std::forward<Args>(args)...); \
        useCtxFlag = false; \
    }

#define SERVER_CALLBACK_WITH_CTX_DEFINE(functionName, callbackMember, ContextFunctorType, useCtxFlag) \
    void functionName(ContextFunctorType functor) { \
        callbackMember = std::move(functor); \
        useCtxFlag = true; \
    }

class Server {
public:
    Server(Looper *looper, InetAddress localAddress)
        : _looper(looper),
          _acceptor(looper, localAddress) {
       
    }

// 定制接口，onNewConnection已有默认实现

    template <typename Functor, typename ...Args>
    void onNewConnection(Functor &&functor, Args &&...args) {
        _acceptor.onNewConnection(std::forward<Functor>(functor), std::forward<Args>(args)...);
    }

    void onNewConnectionWithCtx(AcceptHandler::ContextFunctor functor) {
        _acceptor.onNewConnectionWithCtx(std::move(functor));
    }

// Tcp交互接口

    SERVER_CALLBACK_DEFINE(onConnection,    _connectionCallback, _connectionCallbackUseCtx);
    SERVER_CALLBACK_DEFINE(onMessage,       _messageCallback, _messageCallbackUseCtx);
    SERVER_CALLBACK_DEFINE(onWriteComplete, _writeCompleteCallback, _writeCompleteCallbackUseCtx);
    SERVER_CALLBACK_DEFINE(onClose,         _closeCallback, _closeCallbackUseCtx);

    SERVER_CALLBACK_WITH_CTX_DEFINE(onConnectionWithCtx,    _connectionCallback, TcpHandler::ContextFunctor, _connectionCallbackUseCtx);
    SERVER_CALLBACK_WITH_CTX_DEFINE(onMessageWithCtx,       _messageCallback, TcpHandler::ContextFunctor, _messageCallbackUseCtx);
    SERVER_CALLBACK_WITH_CTX_DEFINE(onWriteCompleteWithCtx, _writeCompleteCallback, TcpHandler::ContextFunctor, _writeCompleteCallbackUseCtx);
    SERVER_CALLBACK_WITH_CTX_DEFINE(onCloseWithCtx,         _closeCallback, TcpHandler::ContextFunctor, _closeCallbackUseCtx);
    


    void start() {
         _acceptor.onNewConnectionWithCtx([this](AcceptContext *ctx) {
            auto connectionInfo = std::move(cast<
                std::pair<Socket, InetAddress>&>(ctx->exchanger));
            Socket &connectionSocket = connectionInfo.first;
            InetAddress &peerAddress = connectionInfo.second;
            auto &connection = _connections.createNewConnection(
                std::move(connectionSocket), ctx->localAddress, peerAddress);
            tcpCallbackInit(connection.get());
        });
    }


private:
    void tcpCallbackInit(TcpHandler*); // 一个乱七八糟的接口


private:
    Pointer<Looper> _looper; // under _looper controll    looper可对付多个Server，也可直接new一个来用，后者需要用shared_ptr/unique_ptr&
    AcceptHandler _acceptor;
    ConnectionPool _connections;

// for TcpHandler

    // TODO 应该造一个variant轮子
    Object _connectionCallback;
    Object _messageCallback;
    Object _writeCompleteCallback;
    Object _closeCallback;

    bool _connectionCallbackUseCtx {false};
    bool _messageCallbackUseCtx {false};
    bool _writeCompleteCallbackUseCtx {false};
    bool _closeCallbackUseCtx {false};
     


};


inline void Server::tcpCallbackInit(TcpHandler *_connection) {
    // FIXME: 由于tcpCtx的this没法提前拿到手，所以目前实现有点别扭
    if(_connectionCallbackUseCtx) {
        _connection->onConnectWithCtx(cast<TcpHandler::ContextFunctor>(_connectionCallback));
    } else {
        _connection->onConnect(cast<LazyEvaluate>(_connectionCallback));
    }

    if(_messageCallbackUseCtx) {
        _connection->onMessageWithCtx(cast<TcpHandler::ContextFunctor>(_messageCallback));
    } else {
        _connection->onMessage(cast<LazyEvaluate>(_messageCallback));
    }

    if(_writeCompleteCallbackUseCtx) {
        _connection->onWriteCompleteWithCtx(cast<TcpHandler::ContextFunctor>(_writeCompleteCallback));
    } else {
        _connection->onWriteComplete(cast<LazyEvaluate>(_writeCompleteCallback));
    }

    if(_closeCallbackUseCtx) {
        _connection->onCloseWithCtx(cast<TcpHandler::ContextFunctor>(_closeCallback));
    } else {
        _connection->onClose(cast<LazyEvaluate>(_closeCallback));
    }
}
#endif