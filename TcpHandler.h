#ifndef __TCP_HANDLER_H__
#define __TCP_HANDLER_H__
#include <bits/stdc++.h>
#include "Handler.h"
#include "Message.h"
#include "Looper.h"
#include "TcpContext.h"
#include "utils/Pointer.h"
#include "utils/Pointer.h"



class TcpHandler: public Handler {
public:

// Handler的日常



    virtual void handle(Message msg) override {
        switch(msg.what) {
            case TcpContext::MSG_POLL_READ:
                handleRead();
            break;
            case TcpContext::MSG_POLL_WRITE:
                handleWrite();
            break;
            case TcpContext::MSG_POLL_ERROR:
                handleError();
            break;
            case TcpContext::MSG_POLL_CLOSE:
                handleClose();
            break;
            default:
            ;
        }
    }


// Callback setter

    // usage:
    //     handler.setOnConnectWithCtx([](TcpContext *ctx) { ctx->setXX(); });
    //     handler.setOnConnect(func, arg0, arg1, arg2);

    HANDLER_CALLBACK_DEFINE(onConnect,       _connectionCallback)
    HANDLER_CALLBACK_DEFINE(onMessage,       _messageCallback)
    HANDLER_CALLBACK_DEFINE(onWriteComplete, _writeCompleteCallback)
    HANDLER_CALLBACK_DEFINE(onClose,         _closeCallback)



    using ContextFunctor = std::function<void(TcpContext*)>;
    void onConnectWithCtx(ContextFunctor functor) { // 由于重载决议的坑，目前先这么用着
        onConnect(std::move(functor), &_ctx);
        // _connectionCallback = _ctx->binder(std::move(functor)); // 不必每个Context都提供这么不可描述的接口
    }
    // TODO onConnectionWithWeakCtx(std::weak_ptr<...> ctx) 提供弱回调支持
    // FIXME: 类型推导时，[](ctx*)会选择HANDLER_CALLBACK_DEFINE，如何让其失败而非错误

// 用于外部定义的函数

    void onMessageWithCtx(ContextFunctor functor) { onMessage(std::move(functor), &_ctx); }
    void onWriteCompleteWithCtx(ContextFunctor functor) { onWriteComplete(std::move(functor), &_ctx); }
    void onCloseWithCtx(ContextFunctor functor) { onClose(std::move(functor), &_ctx); }



    void handleRead() {
        int n; // = buffer.read
        if(n > 0) {
            _messageCallback.evaluate();
        } else if(n == 0) {
            handleClose();
        } else {
            handleError();
        }
    }

    void handleWrite() {
        int n; // = write
    }
    
    void handleError() {
        // throw
    }

    void handleClose() {
        _connectionCallback.evaluate();
        _closeCallback.evaluate();
    }


    TcpHandler(Looper *looper, Socket acceptedSocket,
                InetAddress localAddress, InetAddress peerAddress)
        : _ctx(this, looper, std::move(acceptedSocket), localAddress, peerAddress) { }

protected:
    TcpContext _ctx;

    LazyEvaluate _connectionCallback;
    LazyEvaluate _messageCallback;
    LazyEvaluate _writeCompleteCallback;
    LazyEvaluate _closeCallback;
};
#endif