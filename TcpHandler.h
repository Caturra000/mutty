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

    void handle(Message msg) override {
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
    //     handler.setOnConnect([](TcpContext *ctx) { ctx->setXX(); });
    //     handler.setOnConnect(func, arg0, arg1, arg2);

    using ContextFunctor = std::function<void(TcpContext*)>;
    HANDLER_CALLBACK_DEFINE(onConnect,       _connectionCallback,    ContextFunctor, &_ctx)
    HANDLER_CALLBACK_DEFINE(onMessage,       _messageCallback,       ContextFunctor, &_ctx)
    HANDLER_CALLBACK_DEFINE(onWriteComplete, _writeCompleteCallback, ContextFunctor, &_ctx)
    HANDLER_CALLBACK_DEFINE(onClose,         _closeCallback,         ContextFunctor, &_ctx)

    // TODO onConnectionWithWeakCtx(std::weak_ptr<...> ctx) 提供弱回调支持


// 用于外部定义的函数

    void handleRead() {
        int n = _ctx.inputBuffer.readFrom(_ctx.acceptedSocket.fd());
        if(n > 0) {
            _messageCallback.evaluate();
        } else if(n == 0) { // FIN
            handleClose();
        } else {
            handleError();
        }
    }

    void handleWrite() {
        int n = _ctx.outputBuffer.writeTo(_ctx.acceptedSocket.fd());
        if(n > 0) {
            if(_ctx.outputBuffer.rest() == 0) {
                _ctx.disableWrite();
                _writeCompleteCallback.evaluate(); // shutdown
            }
            // TODO shutdown option
        }
    }
    
    void handleError() {
        throw std::exception();
    }

    void handleClose() {
        _ctx.disableRead();
        _ctx.disableWrite();
        // _connectionCallback.evaluate(); // UNUSED
        _closeCallback.evaluate();
    }


    TcpHandler(Looper *looper, Socket acceptedSocket,
                InetAddress localAddress, InetAddress peerAddress)
        : _ctx(this, looper, std::move(acceptedSocket), localAddress, peerAddress) { }

protected:
    TcpContext _ctx; // TODO 改为shared_ptr，且enable shared from this

    LazyEvaluate _connectionCallback;
    LazyEvaluate _messageCallback;
    LazyEvaluate _writeCompleteCallback;
    LazyEvaluate _closeCallback;
};

#endif