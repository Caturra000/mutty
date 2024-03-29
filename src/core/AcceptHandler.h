#ifndef __MUTTY_ACCEPT_HANDLER_H__
#define __MUTTY_ACCEPT_HANDLER_H__
#include <bits/stdc++.h>
#include "utils/Pointer.h"
#include "base/handler/MessageQueue.h"
#include "base/handler/Handler.h"
#include "net/Socket.h"
namespace mutty {

class AcceptContext;
class AcceptHandler: public Handler {
public:
    void handle(Message msg) override;

    AcceptHandler(AcceptContext* context): _context(context) {}

public:
    Callable _newConnectionCallback;

private:
    void handleRead(int who = 0);

private:
    Pointer<AcceptContext> _context;
};

inline void AcceptHandler::handle(Message msg) {
    switch(msg.what) {
        case ContextImpl::MSG_POLL_READ:
            handleRead(msg.uFlag);
        break;
        default:
        ;
    }
}

} // mutty
#endif