#ifndef __MUTTY_TCP_HANDLER_H__
#define __MUTTY_TCP_HANDLER_H__
#include <bits/stdc++.h>
#include "utils/Pointer.h"
#include "utils/Pointer.h"
#include "base/handler/Handler.h"
#include "base/handler/Message.h"
#include "throws/Exceptions.h"
#include "Looper.h"
namespace mutty {

class TcpContext;
class TcpHandler: public Handler {
public:
    void handle(Message msg) override;

    void handleRead();
    void handleWrite();
    void handleError();
    void handleClose();

    // void init();


    TcpHandler(TcpContext *context): _context(context) {};

    Pointer<TcpContext> _context;
    Callable _connectionCallback;
    Callable _messageCallback;
    Callable _writeCompleteCallback;
    Callable _closeCallback;
};

inline void TcpHandler::handle(Message msg) {
    switch(msg.what) {
        case ContextImpl::MSG_POLL_READ:
            handleRead();
        break;
        case ContextImpl::MSG_POLL_WRITE:
            handleWrite();
        break;
        case ContextImpl::MSG_POLL_ERROR:
            handleError();
        break;
        case ContextImpl::MSG_POLL_CLOSE:
            handleClose();
        break;
        default:
        ;
    }
}



} // mutty

#endif