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
    constexpr static int MSG_TCP_START = 21;

    void handle(Message msg) override;

    TcpHandler(TcpContext *context): _context(context) {};

public:
    Callable _connectionCallback;
    Callable _messageCallback;
    Callable _writeCompleteCallback;
    Callable _closeCallback;

private:
    void handleRead();
    void handleWrite();
    void handleError();
    void handleClose();
    void handleStart();

private:
    Pointer<TcpContext> _context;
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
        case MSG_TCP_START:
            handleStart();
        break;
        default:
        ;
    }
}

} // mutty

#endif