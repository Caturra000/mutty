#ifndef __MUTTY_CONTEXT_H__
#define __MUTTY_CONTEXT_H__
#include <bits/stdc++.h>
#include "utils/NonCopyable.h"
namespace mutty {

class Context: private NonCopyable {
public:
    constexpr static int MSG_POLL_READ  = 1;
    constexpr static int MSG_POLL_WRITE = 2;
    constexpr static int MSG_POLL_ERROR = 3;
    constexpr static int MSG_POLL_CLOSE = 4;

    virtual void sendReadMessage() {}
    virtual void sendWriteMessage() {}
    virtual void sendErrorMessage() {}
    virtual void sendCloseMessage() {}

    virtual int fd() const = 0;
    virtual uint32_t events() const = 0;
    virtual void updateState() = 0;

    virtual void enableRead() {}
    virtual void enableWrite() {}
    virtual void disableRead() {}
    virtual void disableWrite() {}
    virtual bool readEnabled() { return false; }
    virtual bool writeEnabled() { return false;}

    virtual ~Context() {}
};

} // mutty

#endif