#ifndef __MUTTY_TCP_POLICY_H__
#define __MUTTY_TCP_POLICY_H__
#include <bits/stdc++.h>
#include "core/TcpContext.h"
namespace mutty {

#define TCP_POLICY_CALLBACK_DEFINE(callback, policy) \
    template <typename ...Args, typename = IsCallableType<Args...>> \
    void callback(Args &&...args) { \
        policy = cpp11::make_unique<TcpPolicyImpl<Callable>>( \
            Callable::make(std::forward<Args>(args)...)); \
    } \
    template <typename Lambda> \
    void callback(Lambda &&functor) { \
        policy = cpp11::make_unique<TcpPolicyImpl<Lambda>>(std::forward<Lambda>(functor)); \
    }

struct TcpPolicy {
    //virtual void setHandler(TcpHandler*) = 0;
    virtual void onConnect(TcpContext*) = 0;
    virtual void onMessage(TcpContext*) = 0;
    virtual void onWriteComplete(TcpContext*) = 0;
    virtual void onClose(TcpContext*) = 0;
    virtual ~TcpPolicy() {}
};


template <typename T>
struct TcpPolicyImpl: public TcpPolicy {
    typename std::decay<T>::type runtimeInfo;
    TcpPolicyImpl(T info): runtimeInfo(std::move(info)) {}
    void onConnect(TcpContext *connection) override
        { connection->onConnect(runtimeInfo); } // copy
    void onMessage(TcpContext *connection) override
        { connection->onMessage(runtimeInfo); }
    void onWriteComplete(TcpContext *connection) override
        { connection->onWriteComplete(runtimeInfo); }
    void onClose(TcpContext *connection) override
        { connection->onClose(runtimeInfo); }
};

} // mutty

#endif