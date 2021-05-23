#ifndef __MUTTY_ACCEPT_HANDLER_H__
#define __MUTTY_ACCEPT_HANDLER_H__
#include <bits/stdc++.h>
#include "utils/Pointer.h"
#include "base/handler/MessageQueue.h"
#include "base/handler/Handler.h"
#include "net/Socket.h"
#include "AcceptContext.h"
namespace mutty {

class AcceptHandler: public Handler {
public:

    void handle(Message msg) override;
    void handleRead(int who = 0);

    HANDLER_CALLBACK_DEFINE(onNewConnection, _newConnectionCallback, AcceptContext, _ctx)

    AcceptHandler(Looper *looper, InetAddress localAddress)
        : _ctx(std::make_shared<AcceptContext>(this, looper, localAddress)) {}

    void init();

protected:
    std::shared_ptr<AcceptContext> _ctx;
    Callable _newConnectionCallback;
};

inline void AcceptHandler::handle(Message msg) {
    switch(msg.what) {
        case AcceptContext::MSG_POLL_READ:
            handleRead(msg.uFlag);
        break;
        case AcceptContext::MSG_SOCKET_LISTEN:
            // handleListen();
        break;
        default:
        ;
    }
}

inline void AcceptHandler::handleRead(int who) {
    InetAddress peerAddress;
    Socket connectSocket = _ctx->acceptSocket.accept(peerAddress);
    _ctx->exchanger = std::pair<Socket, InetAddress>(
        std::move(connectSocket), std::move(peerAddress));
    _newConnectionCallback(); // add to ConnectionPool of Server
}

inline void AcceptHandler::init() {
    _ctx->acceptSocket.listen();
    _ctx->enableRead();
}

} // mutty
#endif