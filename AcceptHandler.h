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
        // listen
        _newConnectionCallback.evaluate(); // add to ConnectionPool of Server
    }

    HANDLER_CALLBACK_DEFINE(onNewConnection, _newConnectionCallback)
    using ContextFunctor = std::function<void(AcceptContext*)>;
    void onNewConnectionWithCtx(ContextFunctor functor) { 
        onNewConnection(std::move(functor), &_ctx); // FIXME: lazy bind when handle
    }

    // AcceptHandler(Socket *acceptSocket, const InetAddress &localAddress)
    //     : _acceptSocket(acceptSocket), _localAddress(localAddress) {
    // }

protected:
    LazyEvaluate _newConnectionCallback;
    // std::vector<std::unique_ptr<AcceptContext>> _ctxes;
    AcceptContext _ctx;
};
#endif