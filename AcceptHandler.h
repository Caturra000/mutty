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

    void handleRead(int who) {
        InetAddress peerAddress;
        Socket connectSocket = _acceptSocket->accept(peerAddress);
        int id = _ctxes.size();
        _ctxes.emplace_back(std::make_unique<AcceptContext>(
            this, _messageQueue.get(), std::move(connectSocket), _localAddress, peerAddress)); //FIXME: ID
        _newConnectionCallback.evaluate();
    }

    HANDLER_CALLBACK_DEFINE(onNewConnection, _newConnectionCallback)
    using ContextFunctor = std::function<void(AcceptContext*)>;
    void onNewConnectionWithCtx(ContextFunctor functor) { 
        onNewConnection(std::move(functor), _ctxes.back().get()); // FIXME: lazy bind when handle
    }

    AcceptHandler(Socket *acceptSocket, const InetAddress &localAddress)
        : _acceptSocket(acceptSocket), _localAddress(localAddress) {
    }

protected:
    Pointer<Socket> _acceptSocket;
    InetAddress _localAddress;
    LazyEvaluate _newConnectionCallback;
    Pointer<MessageQueue> _messageQueue;
    std::vector<std::unique_ptr<AcceptContext>> _ctxes;
};
#endif