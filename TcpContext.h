#ifndef __TCP_CONTEXT_H__
#define __TCP_CONTEXT_H__
#include <bits/stdc++.h>
#include "Socket.h"
#include "InetAddress.h"
#include "Buffer.h"
#include "Timer.h"
#include "Message.h"
#include "MessageQueue.h"
#include "utils/LazyEvaluate.h"
#include "utils/Pointer.h"
#include "Context.h"

// 按照逻辑上，应该由Accept过程生成
class TcpHandler;
class TcpContext: public Context {
public:

// MESSAGE定制

    CONTEXT_MSG_DEFINE(MSG_ON_CONNECT);
    CONTEXT_MSG_DEFINE(MSG_ON_MESSAGE);
    CONTEXT_MSG_DEFINE(MSG_ON_WRITE_COMPLETE);
    CONTEXT_MSG_DEFINE(MSG_ON_CLOSE);

// 网络特性支持

    Socket acceptedSocket;
    InetAddress localInet, peerInet;
    Buffer inputBuffer, outputBuffer;

// 定时器特性支持

    Pointer<Timer> timer; // 如果需要用到定时器，只需添加这个即可


    // 适配器，绑定当前ctx，让一个functor(ctx)适配为LazyEvaluate
    // 从Pointer换回raw ptr
    LazyEvaluate binder(std::function<void(TcpContext*)> functor) {
        return LazyEvaluate::lazy(std::move(functor), this);
    }

    // void sendReadMessage() override { _messagequeue->post({static_cast<Handler*>(_handler.get()), MSG_ON_MESSAGE});}
    void sendReadMessage() override { _messagequeue->post({_handler.castTo<Handler>(), MSG_ON_MESSAGE});}

    int fd() const override { return acceptedSocket.fd(); }
    int events() const override { return 0; }

    TcpContext(Pointer<TcpHandler> handler)
        : _handler(handler) { }


protected:
    Pointer<TcpHandler> _handler; // master
    Pointer<MessageQueue> _messagequeue; // post to mq
};
#endif