#ifndef __TCP_CONTEXT_H__
#define __TCP_CONTEXT_H__
#include <bits/stdc++.h>
#include "Socket.h"
#include "InetAddress.h"
#include "Buffer.h"
#include "BufferPool.h"
#include "Timer.h"
#include "Message.h"
#include "MessageQueue.h"
#include "utils/LazyEvaluate.h"
#include "utils/Pointer.h"
#include "ContextImpl.h"

// 按照逻辑上，应该由Accept过程生成
class TcpHandler;
class TcpContext: public ContextImpl {
public:

// MESSAGE定制

    // CONTEXT_MSG_DEFINE(MSG_POLL_READ);
    // CONTEXT_MSG_DEFINE(MSG_POLL_WRITE);
    // CONTEXT_MSG_DEFINE(MSG_POLL_ERROR);
    // CONTEXT_MSG_DEFINE(MSG_POLL_CLOSE);
    

// 网络特性支持

    Socket acceptedSocket;
    InetAddress localAddress, peerAddress;
    CachedBuffer inputBuffer, outputBuffer;

// 定时器特性支持

    Pointer<Timer> timer; // 如果需要用到定时器，只需添加这个即可


    // 适配器，绑定当前ctx，让一个functor(ctx)适配为LazyEvaluate
    // 从Pointer换回raw ptr
    LazyEvaluate binder(std::function<void(TcpContext*)> functor) {
        return LazyEvaluate::lazy(std::move(functor), this);
    }
    

// ONLY FOR POLLER

    int fd() const override { return acceptedSocket.fd(); }


    TcpContext(Handler *handler, Looper *looper, 
            Socket acceptedSocket, InetAddress localAddress, InetAddress peerAddress)
        : ContextImpl(handler, looper),
          acceptedSocket(std::move(acceptedSocket)),
          localAddress(localAddress),
          peerAddress(peerAddress) {}

};
#endif