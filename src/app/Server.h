#ifndef __SERVER_H__
#define __SERVER_H__
#include <bits/stdc++.h>
#include "utils/Pointer.h"
#include "utils/Exchanger.h"
#include "utils/Object.h"
#include "utils/TypeTraits.h"
#include "utils/Compat.h"
#include "base/handler/MessageQueue.h"
#include "core/Looper.h"
#include "core/AcceptHandler.h"
#include "core/ConnectionPool.h"
#include "core/TcpHandler.h"

#define SERVER_CALLBACK_DEFINE(functionName, callbackMember, useCtxFlag) \
    template <typename ...Args, typename = IsCallableType<Args...>> \
    void functionName(Args &&...args) { \
        callbackMember = LazyEvaluate::lazy(std::forward<Args>(args)...); \
        useCtxFlag = false; \
    }

#define SERVER_CALLBACK_WITH_CTX_DEFINE(functionName, callbackMember, ContextFunctorType, useCtxFlag) \
    void functionName(ContextFunctorType &&functor) { \
        callbackMember = std::forward<ContextFunctorType>(functor); \
        useCtxFlag = true; \
    }

class Server {
public:
    Server(Looper *looper, InetAddress localAddress)
        : _looper(looper),
          _acceptor(looper, localAddress) {
       
    }

// 定制接口，onNewConnection已有默认实现

    template <typename Functor, typename ...Args, typename = IsCallableType<Functor, Args...>>
    void onNewConnection(Functor &&functor, Args &&...args) {
        _acceptor.onNewConnection(std::forward<Functor>(functor), std::forward<Args>(args)...);
    }

    void onNewConnection(std::function<void(AcceptContext*)> functor) {
        _acceptor.onNewConnection(std::move(functor));
    }

// Tcp交互接口

    // TODO 简化macro，并提供weak callback支持

    SERVER_CALLBACK_DEFINE(onConnect,    _connectionCallback, _connectionCallbackUseCtx);
    SERVER_CALLBACK_DEFINE(onMessage,       _messageCallback, _messageCallbackUseCtx);
    SERVER_CALLBACK_DEFINE(onWriteComplete, _writeCompleteCallback, _writeCompleteCallbackUseCtx);
    SERVER_CALLBACK_DEFINE(onClose,         _closeCallback, _closeCallbackUseCtx);

    SERVER_CALLBACK_WITH_CTX_DEFINE(onConnect,    _connectionCallback, TcpHandler::ContextFunctor, _connectionCallbackUseCtx);
    SERVER_CALLBACK_WITH_CTX_DEFINE(onMessage,       _messageCallback, TcpHandler::ContextFunctor, _messageCallbackUseCtx);
    SERVER_CALLBACK_WITH_CTX_DEFINE(onWriteComplete, _writeCompleteCallback, TcpHandler::ContextFunctor, _writeCompleteCallbackUseCtx);
    SERVER_CALLBACK_WITH_CTX_DEFINE(onClose,         _closeCallback, TcpHandler::ContextFunctor, _closeCallbackUseCtx);
    


    void start() {
         _acceptor.onNewConnection([this](AcceptContext *ctx) {
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

    struct PolicyBase {
        //virtual void setHandler(TcpHandler*) = 0;
        virtual void onConnect(TcpHandler*) = 0;
        virtual void onMessage(TcpHandler*) = 0;
        virtual void onWriteComplete(TcpHandler*) = 0;
        virtual void onClose(TcpHandler*) = 0;
        virtual ~PolicyBase() { }
    };

    std::unique_ptr<PolicyBase> _connectPolicy;
    std::unique_ptr<PolicyBase> _messagePolicy;
    std::unique_ptr<PolicyBase> _writeCompletePolicy;
    std::unique_ptr<PolicyBase> _closePolicy;

    template <typename T>
    struct Policy: public PolicyBase {
        //using DecayT = typename std::decay<T>::type;
        T runtimeInfo;
        Policy(T info): runtimeInfo(std::move(info)) {}
        void onConnect(TcpHandler *connection) override
            { connection->onConnect(std::move(runtimeInfo));} // call once
        void onMessage(TcpHandler *connection) override
            { connection->onMessage(std::move(runtimeInfo));}
        void onWriteComplete(TcpHandler *connection) override
            { connection->onWriteComplete(std::move(runtimeInfo));}
        void onClose(TcpHandler *connection) override
            { connection->onClose(std::move(runtimeInfo));} 
    } ;
public:
// experimental

    template <typename ...Args, typename = IsCallableType<Args...>>
    void onConnect000(Args &&...args) {
        _connectPolicy = cpp11::make_unique<Policy<LazyEvaluate>>(
            LazyEvaluate::lazy(std::forward<Args>(args)...));
    }
    void onConnect000(std::function<void(TcpContext*)> functor) {
        using TLDR = std::function<void(TcpContext*)>;
        _connectPolicy = cpp11::make_unique<Policy<TLDR>>(std::move(functor));
    }
    void onConnect000(std::function<void(std::weak_ptr<TcpContext>)> functor) {
        using TLDR = std::function<void(std::weak_ptr<TcpContext>)>;
        _connectPolicy = cpp11::make_unique<Policy<TLDR>>(std::move(functor));
    }

};

// TODO 通过Policy来解决这种垃圾分类
inline void Server::tcpCallbackInit(TcpHandler *_connection) {
    // FIXME: 由于tcpCtx的this没法提前拿到手，所以目前实现有点别扭
    if(_connectPolicy) _connectPolicy->onConnect(_connection);

    if(_connectionCallbackUseCtx) {
        _connection->onConnect(cast<TcpHandler::ContextFunctor>(_connectionCallback));
    } else {
        _connection->onConnect(cast<LazyEvaluate>(_connectionCallback));
    }

    if(_messageCallbackUseCtx) {
        _connection->onMessage(cast<TcpHandler::ContextFunctor>(_messageCallback));
    } else {
        _connection->onMessage(cast<LazyEvaluate>(_messageCallback));
    }

    if(_writeCompleteCallbackUseCtx) {
        _connection->onWriteComplete(cast<TcpHandler::ContextFunctor>(_writeCompleteCallback));
    } else {
        _connection->onWriteComplete(cast<LazyEvaluate>(_writeCompleteCallback));
    }

    if(_closeCallbackUseCtx) {
        _connection->onClose(cast<TcpHandler::ContextFunctor>(_closeCallback));
    } else {
        _connection->onClose(cast<LazyEvaluate>(_closeCallback));
    }
}
#endif