#ifndef __TCP_HANDLER_H__
#define __TCP_HANDLER_H__
#include <bits/stdc++.h>
#include "base/handler/Handler.h"
#include "base/handler/Message.h"
#include "Looper.h"
#include "TcpContext.h"
#include "utils/Pointer.h"
#include "utils/Pointer.h"
#include "throws/Exceptions.h"


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
    //     handler.onConnect([](TcpContext *ctx) { ctx->setXX(); });
    //     handler.onConnect(func, arg0, arg1, arg2);
    //     handler.onConnect([](std::weak_ptr<TcpContext> ctx)) {};

    HANDLER_CALLBACK_DEFINE(onConnect,       _connectionCallback,    TcpContext, _ctx)
    HANDLER_CALLBACK_DEFINE(onMessage,       _messageCallback,       TcpContext, _ctx)
    HANDLER_CALLBACK_DEFINE(onWriteComplete, _writeCompleteCallback, TcpContext, _ctx)
    HANDLER_CALLBACK_DEFINE(onClose,         _closeCallback,         TcpContext, _ctx)


// 用于外部定义的函数

    void handleRead() {
        int n = _ctx->inputBuffer.readFrom(_ctx->acceptedSocket.fd());
        if(n > 0) {
            _messageCallback();
        } else if(n == 0) { // FIN
            handleClose();
        } else {
            handleError();
        }
    }

    void handleWrite() {
        int n = _ctx->outputBuffer.writeTo(_ctx->acceptedSocket.fd()); // can async?
        if(n > 0) {
            if(_ctx->outputBuffer.rest() == 0) {
                _ctx->disableWrite();
                _writeCompleteCallback();
            }
        }
    }
    
    void handleError() {
        throw MuttyException("error callback");
    }

    void handleClose() {
        if(_ctx->isConnecting() || _ctx->isDisConnecting()) {
            _ctx->disableRead();
            _ctx->disableWrite();
            _ctx->networkStatus = TcpContext::DISCONNECTED;
            _closeCallback();
        }
        
    }


    TcpHandler(Looper *looper, Socket acceptedSocket,
                InetAddress localAddress, InetAddress peerAddress)
        : _ctx(std::make_shared<TcpContext>(
            this, looper, std::move(acceptedSocket), localAddress, peerAddress)) { }

protected:
    std::shared_ptr<TcpContext> _ctx;

    Callable _connectionCallback;
    Callable _messageCallback;
    Callable _writeCompleteCallback;
    Callable _closeCallback;
};

#endif