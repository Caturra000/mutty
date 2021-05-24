#ifndef __MUTTY_TCP_CONTEXT_H__
#define __MUTTY_TCP_CONTEXT_H__
#include <bits/stdc++.h>
#include "utils/Callable.h"
#include "utils/Pointer.h"
#include "base/buffer/Buffer.h"
#include "base/buffer/BufferPool.h"
#include "base/timer/Timer.h"
#include "base/handler/Message.h"
#include "base/handler/MessageQueue.h"
#include "net/Socket.h"
#include "net/InetAddress.h"
#include "ContextImpl.h"
#include "TcpHandler.h"
namespace mutty {


class TcpContext: public ContextImpl, public std::enable_shared_from_this<TcpContext> {
public:
    bool isConnecting() { return networkStatus == CONNECTING; }
    bool isConnected() { return networkStatus == CONNECTED; }
    bool isDisConnecting() { return networkStatus == DISCONNECTING; }
    bool isDisConnected() { return networkStatus == DISCONNECTED; }

    void shutdown(/*bool force = false*/);
    void forceClose();
    void forceClose(Nanosecond delay);

    template <size_t N>
    void send(const char (&data)[N]) { send(data, N); }
    void send(const std::string &str) { send(str.c_str(), str.length()); }
    void send(const void *data, int length);

    int fd() const override { return acceptedSocket.fd(); }

    void start();

    Callable binder(std::function<void(TcpContext*)> functor);

    HANDLER_CALLBACK_DEFINE(onConnect,       _handler._connectionCallback,    TcpContext, this)
    HANDLER_CALLBACK_DEFINE(onMessage,       _handler._messageCallback,       TcpContext, this)
    HANDLER_CALLBACK_DEFINE(onWriteComplete, _handler._writeCompleteCallback, TcpContext, this)
    HANDLER_CALLBACK_DEFINE(onClose,         _handler._closeCallback,         TcpContext, this)

    TcpContext(Looper *looper, Socket acceptedSocket,
               InetAddress localAddress, InetAddress peerAddress);

public:
    enum NetworkStatus { CONNECTING, CONNECTED, DISCONNECTING, DISCONNECTED };

    Socket acceptedSocket;
    InetAddress localAddress, peerAddress;
    CachedBuffer inputBuffer, outputBuffer;
    Pointer<Timer> scheduler;

private:
    void setConnecting() { networkStatus = CONNECTING; }
    void setConnected() { networkStatus = CONNECTED; }
    void setDisConnecting() { networkStatus = DISCONNECTING; }
    void setDisConnected() { networkStatus = DISCONNECTED; }

private:
    TcpHandler _handler;
    NetworkStatus networkStatus { CONNECTING };

    friend class TcpHandler;
};

inline void TcpContext::shutdown(/*bool force = false*/) {
    if(/*force || */isConnected()) {
        setDisConnecting();
        if(_events & EVENT_WRITE) {
            acceptedSocket.shutdown();
        }
    }
}

inline void TcpContext::forceClose() {
    if(isConnected() || isDisConnecting()) {
        sendCloseMessage();
    }
}

inline void TcpContext::forceClose(Nanosecond delay) {
    std::weak_ptr<TcpContext> _this = shared_from_this();
    scheduler->runAfter(delay).with([_this] { 
        if(auto context = _this.lock()) {
            context->forceClose();
        }
    });
}

inline void TcpContext::send(const void *data, int length) {
    if(!isDisConnecting() || !isDisConnected()) {
        outputBuffer.append(static_cast<const char *>(data), length);
        if(!(_events & EVENT_WRITE)) enableWrite();
    }
}

inline void TcpContext::start() {
    std::weak_ptr<TcpContext> _this = shared_from_this();
    sendMessage(TcpHandler::MSG_TCP_START);
}

inline Callable TcpContext::binder(std::function<void(TcpContext*)> functor) {
    return Callable::make(std::move(functor), this);
}

inline TcpContext::TcpContext(Looper *looper, Socket acceptedSocket,
        InetAddress localAddress, InetAddress peerAddress)
    : ContextImpl(&_handler, looper),
      _handler(this),
      acceptedSocket(std::move(acceptedSocket)),
      localAddress(localAddress),
      peerAddress(peerAddress),
      scheduler(looper ? looper->getScheduler() : nullptr) {}
} // mutty
#endif