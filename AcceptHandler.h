#ifndef __ACCEPT_HANDLER_H__
#define __ACCEPT_HANDLER_H__
#include "bits/stdc++.h"
#include "utils/Pointer.h"
#include "Handler.h"
#include "AcceptContext.h"
#include "Socket.h"
#include "MessageQueue.h"

class AcceptHandler: public Handler {
public:

    void handle(Message msg) override {
        switch(msg.what) {
            case AcceptContext::MSG_POLL_READ:
                handleRead(msg.flag);
            break;
            case AcceptContext::MSG_SOCKET_LISTEN:
                // handleListen();
            break;
            default:
            ;
        }
    }

    // UNUSED who
    void handleRead(int who = 0) {
        InetAddress peerAddress;
        Socket connectSocket = _ctx.acceptSocket.accept(peerAddress);
        _ctx.exchanger = std::pair<Socket, InetAddress>(
            std::move(connectSocket), std::move(peerAddress));
        _newConnectionCallback.evaluate(); // add to ConnectionPool of Server
    }

    HANDLER_CALLBACK_DEFINE(onNewConnection, _newConnectionCallback)
    using ContextFunctor = std::function<void(AcceptContext*)>;
    void onNewConnectionWithCtx(ContextFunctor functor) { 
        onNewConnection(std::move(functor), &_ctx);
    }

    AcceptHandler(Looper *looper, Socket acceptSocket, InetAddress localAddress)
        : _ctx(this, looper, std::move(acceptSocket), localAddress) {} // TODO bind listen


    void listen() {
        _ctx.acceptSocket.listen();
        // TODO _ctx.enableRead();
    }

protected:
    AcceptContext _ctx;

    LazyEvaluate _newConnectionCallback;
};
#endif