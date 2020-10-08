#ifndef __TCP_HANDLER_H__
#define __TCP_HANDLER_H__
#include <bits/stdc++.h>
#include "Handler.h"
#include "Message.h"
#include "TcpFeature.h"
#include "TcpContext.h"
#include "utils/Pointer.h"
#include "utils/Pointer.h"



class TcpHandler: public Handler, public TcpFeature {
public:

// Handler的日常

    // FIXME:按照现在的趋势，MSG应该在Context层声明比较合适
    HANDLER_MSG_DEFINE(MSG_TCP_ON_ACCEPT);

    virtual void handle(Message msg) override {
        switch(msg.what) {
            case MSG_TCP_ON_ACCEPT:
            break;
            default:
            break;
        }
    }


// Callback setter

    // usage:
    //     handler.setOnConnectWithCtx([](TcpContext *ctx) { ctx->setXX(); });
    //     handler.setOnConnect(func, arg0, arg1, arg2);
    using ContextFunctor = std::function<void(TcpContext*)>;
    void onConnectWithCtx(ContextFunctor functor) { // 由于重载决议的坑，目前先这么用着
        onConnect(std::move(functor), _ctx.get());
        // _connectionCallback = _ctx->binder(std::move(functor));
    }

    // FIXME: 类型推导时，[](ctx*)会选择HANDLER_CALLBACK_DEFINE，如何让其失败而非错误

    HANDLER_CALLBACK_DEFINE(onConnect, _connectionCallback);





protected:
    std::unique_ptr<TcpContext> _ctx;

    LazyEvaluate _connectionCallback;
    LazyEvaluate _messageCallback;
    LazyEvaluate _writeCompleteCallback;
    LazyEvaluate _closeCallback;
};
#endif