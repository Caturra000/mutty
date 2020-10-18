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
        auto connectionInfo = new std::pair<Socket, InetAddress>(
            std::move(connectSocket), std::move(peerAddress));
        // _ctx.sendMessageWithData(AcceptContext::MSG_ACCEPT_WITH_DATA, connectionInfo);  // FIXME: 逻辑上的矛盾，如果改动ctx的send接口允许任意handler又显得结构非常混乱
        // TODO 具有跨Handler/Context语义的操作我觉得用Broadcast做一个observer模式比较合适，毕竟Message只适合针对性的发送
        _newConnectionCallback.evaluate(); // add to ConnectionPool of Server
        // FIXME 不希望callback(socket, inet)的形式提供setter， 但目前MSG是否值得为了跨Handler而多增设一个any/sp<void*>?这会增加整个MSG通信成本 MSG也要多态的话更不应该了
        // 目前成本最低的做法是临时提供一个额外的回调接受(socket,inet)
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