#ifndef __CONTEXT_H__
#define __CONTEXT_H__
#include <bits/stdc++.h>

// 命名规范： MSG_<Message>
#define CONTEXT_MSG_DEFINE(messageType) \
    const static int messageType = __COUNTER__+1 

// 只提供接口部分，该接口会应用于IO复用
// 虽然设计上是都会post到某个MQ，但作为接口，持有MQ的指针或者send(MQ)都不太必要，因此多提供一个ContextImpl
class Context {
public:

    CONTEXT_MSG_DEFINE(MSG_POLL_READ);
    CONTEXT_MSG_DEFINE(MSG_POLL_WRITE);
    CONTEXT_MSG_DEFINE(MSG_POLL_ERROR);
    CONTEXT_MSG_DEFINE(MSG_POLL_CLOSE);

    virtual void sendReadMessage() {}
    virtual void sendWriteMessage() {}
    virtual void sendErrorMessage() {}
    virtual void sendCloseMessage() {}

// poll

    virtual int fd() const = 0;
    virtual uint32_t events() const = 0;
    virtual int state() const = 0;

    virtual void updateStatus() {}
    virtual void enableRead() {}
    virtual void enableWrite() {}
    virtual void disableRead() {}
    virtual void disableWrite() {}
    // update
    // add remove change
    // status
};

// using ContextImpl = Context;



#endif