#ifndef __CONNECT_HANDLER_H__
#define __CONNECT_HANDLER_H__
#include <bits/stdc++.h>
#include <errno.h>
#include "utils/Callable.h"
#include "utils/Timestamp.h"
#include "base/handler/Handler.h"
#include "base/handler/MessageQueue.h"
#include "base/context/Context.h"
#include "core/ConnectContext.h"
#include "net/Socket.h"

class ConnectHandler: public Handler {
public:
    // TODO MSG_DEFINE不必CONTEXT前缀
    CONTEXT_MSG_DEFINE(MSG_CONNECT_START)
    
    void handle(Message msg) override {
        switch(msg.what) {
            default:;
        }
    }

    ConnectHandler(Looper *looper)
        : _ctx(std::make_shared<ConnectContext>(this, looper)),
          _messageQueue(looper ? looper->getProvider() : nullptr) {}

    HANDLER_CALLBACK_DEFINE(onNewConnection, _newConnectionCallback, ConnectContext, _ctx)


    void start() { _messageQueue->post({this, MSG_CONNECT_START}); }
    void restart() { _retryInterval = 50ms; }
    void stop() {}

    // handleStart
    void connect() {
        Socket socket;
        // TODO socket.connect();
        int errno;

    }
    void connecting(int fd) {}

    void handleWrite() {}
    void handleError() {}

    // 还未存在ctx
    void retry(int fd) {
    }

    void resetContext() { _ctx.reset(); }

private:
    std::shared_ptr<ConnectContext> _ctx;
    Callable _newConnectionCallback;
    Millisecond _retryInterval {50ms};
    Pointer<MessageQueue> _messageQueue;
    
}
#endif