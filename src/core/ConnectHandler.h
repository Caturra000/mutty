#ifndef __CONNECT_HANDLER_H__
#define __CONNECT_HANDLER_H__
#include <bits/stdc++.h>
#include "utils/LazyEvaluate.h"
#include "core/ConnectContext.h"

class ConnectHandler: public Handler {
public:
    void handle(Message msg) override {
        switch(msg.what) {
            default:;
        }
    }

    ConnectHandler(Looper *looper)
        : _ctx(std::make_shared<ConnectContext>(this, looper)) {}

    HANDLER_CALLBACK_DEFINE(onNewConnection, _newConnectionCallback, ConnectContext, _ctx)
private:
    std::shared_ptr<ConnectContext> _ctx;
    LazyEvaluate _newConnectionCallback;
}
#endif