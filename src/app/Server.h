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


#define POLICY_CALLBACK_DEFINE(callback, policy, ContextType) \
    template <typename ...Args, typename = IsCallableType<Args...>> \
    void callback(Args &&...args) { \
        policy = cpp11::make_unique<PolicyImpl<LazyEvaluate>>( \
            LazyEvaluate::lazy(std::forward<Args>(args)...)); \
    } \
    void callback(std::function<void(ContextType*)> functor) { \
        using TLDR = std::function<void(ContextType*)>; \
        policy = cpp11::make_unique<PolicyImpl<TLDR>>(std::move(functor)); \
    } \
    void callback(std::function<void(std::weak_ptr<ContextType>)> functor) { \
        using TLDR = std::function<void(std::weak_ptr<ContextType>)>; \
        policy = cpp11::make_unique<PolicyImpl<TLDR>>(std::move(functor)); \
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
    void tcpCallbackInit(TcpHandler*);


private:
    Pointer<Looper> _looper;
    AcceptHandler _acceptor;
    ConnectionPool _connections;

// for TcpHandler

    // 用于泛型擦除
    struct Policy {
        //virtual void setHandler(TcpHandler*) = 0;
        virtual void onConnect(TcpHandler*) = 0;
        virtual void onMessage(TcpHandler*) = 0;
        virtual void onWriteComplete(TcpHandler*) = 0;
        virtual void onClose(TcpHandler*) = 0;
        virtual ~Policy() { }
    };

    // 使用unique_ptr提供RAII和多态

    std::unique_ptr<Policy> _connectPolicy;
    std::unique_ptr<Policy> _messagePolicy;
    std::unique_ptr<Policy> _writeCompletePolicy;
    std::unique_ptr<Policy> _closePolicy;

    template <typename T>
    struct PolicyImpl: public Policy {
        //using DecayT = typename std::decay<T>::type;
        T runtimeInfo;
        PolicyImpl(T info): runtimeInfo(std::move(info)) {}
        void onConnect(TcpHandler *connection) override
            { connection->onConnect(runtimeInfo);} // copy
        void onMessage(TcpHandler *connection) override
            { connection->onMessage(runtimeInfo);}
        void onWriteComplete(TcpHandler *connection) override
            { connection->onWriteComplete(runtimeInfo);}
        void onClose(TcpHandler *connection) override
            { connection->onClose(runtimeInfo);} 
    } ;
public:

    POLICY_CALLBACK_DEFINE(onConnect, _connectPolicy, TcpContext)
    POLICY_CALLBACK_DEFINE(onMessage, _messagePolicy, TcpContext)
    POLICY_CALLBACK_DEFINE(onWriteComplete, _writeCompletePolicy, TcpContext)
    POLICY_CALLBACK_DEFINE(onClose, _closePolicy, TcpContext)
    

};

inline void Server::tcpCallbackInit(TcpHandler *connection) {
    if(_connectPolicy) _connectPolicy->onConnect(connection);
    if(_messagePolicy) _messagePolicy->onMessage(connection);
    if(_writeCompletePolicy) _writeCompletePolicy->onWriteComplete(connection);
    if(_closePolicy) _closePolicy->onClose(connection);
}
#endif