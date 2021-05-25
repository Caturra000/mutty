#ifndef __MUTTY_ACCEPT_CONTEXT_H__
#define __MUTTY_ACCEPT_CONTEXT_H__
#include <fcntl.h>
#include "utils/Pointer.h"
#include "utils/Exchanger.h"
#include "net/Socket.h"
#include "net/InetAddress.h"
#include "ContextImpl.h"
#include "AcceptHandler.h"
namespace mutty {

class AcceptContext: public ContextImpl, public std::enable_shared_from_this<AcceptContext> {
public:
    int fd() const override { return acceptSocket.fd(); }
    std::unique_ptr<std::pair<Socket, InetAddress>> poll();
    void start();

    HANDLER_CALLBACK_DEFINE(onNewConnection, _handler._newConnectionCallback, AcceptContext, this)

    AcceptContext(Looper *looper, InetAddress localAddress);

public:
    Socket acceptSocket;
    InetAddress localAddress;
    Exchanger exchanger; // deprecated

private:
    AcceptHandler _handler;
    std::unique_ptr<std::pair<Socket, InetAddress>> _connectionInfo;

    friend class AcceptHandler;
};

inline std::unique_ptr<std::pair<Socket, InetAddress>> AcceptContext::poll() {
    return std::move(_connectionInfo);
}

inline void AcceptContext::start() {
    acceptSocket.listen();
    enableRead();
}

inline AcceptContext::AcceptContext(Looper *looper, InetAddress localAddress)
    : ContextImpl(&_handler, looper),
      localAddress(localAddress),
      _handler(this) {
    using Option = Socket::Option;
    acceptSocket.config(Option::REUSE_PORT | Option::REUSE_ADDR);
    acceptSocket.bind(localAddress);
}

} // mutty
#endif