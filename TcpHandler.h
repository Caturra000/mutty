#ifndef __TCP_HANDLER_H__
#define __TCP_HANDLER_H__
#include <bits/stdc++.h>
#include "Handler.h"
#include "Message.h"
#include "TcpContext.h"
#include "utils/Pointer.h"
#include "utils/Pointer.h"



class TcpHandler: public Handler {
public:

// Handler的日常

    virtual void handle(Message msg) override {
        switch(msg.what) {
            case TcpContext::MSG_ON_CONNECT:
                _connectionCallback.evaluate();
            break;
            case TcpContext::MSG_ON_MESSAGE:
                _messageCallback.evaluate();
            break;
            case TcpContext::MSG_ON_WRITE_COMPLETE:
                _writeCompleteCallback.evaluate();
            break;
            case TcpContext::MSG_ON_CLOSE:
                _closeCallback.evaluate();
            break;
            default:
        }
    }


// Callback setter

    // usage:
    //     handler.setOnConnectWithCtx([](TcpContext *ctx) { ctx->setXX(); });
    //     handler.setOnConnect(func, arg0, arg1, arg2);

    HANDLER_CALLBACK_DEFINE(onConnect,       _connectionCallback);
    HANDLER_CALLBACK_DEFINE(onMessage,       _messaageCallback);
    HANDLER_CALLBACK_DEFINE(onWriteComplete, _writeCompleteCallback);
    HANDLER_CALLBACK_DEFINE(onClose,         _closeCallback);



    using ContextFunctor = std::function<void(TcpContext*)>;
    void onConnectWithCtx(ContextFunctor functor) { // 由于重载决议的坑，目前先这么用着
        onConnect(std::move(functor), _ctx.get());
        // _connectionCallback = _ctx->binder(std::move(functor)); // 不必每个Context都提供这么不可描述的接口
    }

    // FIXME: 类型推导时，[](ctx*)会选择HANDLER_CALLBACK_DEFINE，如何让其失败而非错误

    void onMessageWithCtx(ContextFunctor functor) { onMessage(std::move(functor), _ctx.get()); }
    void onWriteCompleteWithCtx(ContextFunctor functor) { onWriteComplete(std::move(functor), _ctx.get()); }
    void onCloseWithCtx(ContextFunctor functor) { onClose(std::move(functor), _ctx.get()); }
    




protected:
    std::unique_ptr<TcpContext> _ctx;

    LazyEvaluate _connectionCallback;
    LazyEvaluate _messageCallback;
    LazyEvaluate _writeCompleteCallback;
    LazyEvaluate _closeCallback;
};
#endif