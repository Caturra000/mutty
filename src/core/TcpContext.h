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
#include "log/Log.h"
#include "ContextImpl.h"
#include "TcpHandler.h"
namespace mutty {


class TcpContext: public ContextImpl, public std::enable_shared_from_this<TcpContext> {
public:
    bool isConnecting() const { return _networkStatus == CONNECTING; }
    bool isConnected() const { return _networkStatus == CONNECTED; }
    bool isDisConnecting() const { return _networkStatus == DISCONNECTING; }
    bool isDisConnected() const { return _networkStatus == DISCONNECTED; }

    void shutdown(/*bool force = false*/);
    void forceClose();
    void forceClose(Nanosecond delay);

    template <size_t N>
    void send(const char (&data)[N]) { send(data, N); }
    void send(const std::string &str) { send(str.c_str(), str.length()); }
    void send(const void *data, ssize_t length);
    template <typename ...Args>
    void sendAsync(Args &&...args); // helper

    int fd() const override { return acceptedSocket.fd(); }

    void start();

    const std::string& simpleInfo() const;
    const char* networkInfo() const;
    uint64_t hashcode() const;

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
    std::exception_ptr exception;

private:
    void setConnecting() { _networkStatus = CONNECTING; }
    void setConnected() { _networkStatus = CONNECTED; }
    void setDisConnecting() { _networkStatus = DISCONNECTING; }
    void setDisConnected() { _networkStatus = DISCONNECTED; }

private:
    TcpHandler _handler;
    NetworkStatus _networkStatus { CONNECTING };
    mutable std::string _cachedInfo;
    mutable uint64_t _hashcode {};

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

inline void TcpContext::send(const void *data, ssize_t length) {
    if(!isDisConnecting() || !isDisConnected()) {
        outputBuffer.append(static_cast<const char *>(data), length);
        if(!(_events & EVENT_WRITE)) enableWrite();
    }
}

template <typename ...Args>
inline void TcpContext::sendAsync(Args &&...args) {
    if(!isDisConnecting() || !isDisConnected()) {
        async([=] { send(args...); }); // copy
    }
}

inline void TcpContext::start() {
    MUTTY_LOG_INFO("new connection, try to start, hash =", hashcode());
    sendMessage(TcpHandler::MSG_TCP_START);
}

inline const std::string& TcpContext::simpleInfo() const {
    if(_cachedInfo.length()) return _cachedInfo;
    return _cachedInfo = "socket fd = " + std::to_string(acceptedSocket.fd())
        + ", local = " + localAddress.toString() + ", peer = " + peerAddress.toString();
}

inline const char* TcpContext::networkInfo() const {
    switch(_networkStatus) {
        case CONNECTING: return "CONNECTING";
        case CONNECTED: return "CONNECTED";
        case DISCONNECTING: return "DISCONNECTING";
        case DISCONNECTED: return "DISCONNECTED";
        default: return "?";
    }
}

inline uint64_t TcpContext::hashcode() const {
    if(_hashcode) return _hashcode;
    return _hashcode = (uint64_t(acceptedSocket.fd()) << 32)
        ^ uint64_t(localAddress.rawIp())
        ^ uint64_t(peerAddress.rawIp())
        ^ (uint64_t(localAddress.rawPort()) << 24)
        ^ (uint64_t(peerAddress.rawPort()) << 24);
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