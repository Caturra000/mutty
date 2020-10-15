#ifndef __CONTEXT_IMPL_H__
#define __CONTEXT_IMPL_H__
#include <bits/stdc++.h>
#include "Context.h"
#include "MessageQueue.h"
#include "Message.h"
#include "utils/Pointer.h"
#include "Looper.h"

class Handler;

// 用于简单实现的包装类，如果是相对复杂的实现，应该直接继承Context，并且明确Handler是哪种类型
class ContextImpl: public Context {
public:
    void sendMessage(int what) { _messageQueue->post({_handler, what}); } 

    void sendReadMessage() override  { sendMessage(MSG_POLL_READ);  }
    void sendWriteMessage() override { sendMessage(MSG_POLL_WRITE); }
    void sendErrorMessage() override { sendMessage(MSG_POLL_ERROR); }
    void sendCloseMessage() override { sendMessage(MSG_POLL_CLOSE); }

    ContextImpl(Handler *handler = nullptr, Looper *looper = nullptr)
        : _handler(handler),
          _messageQueue(looper ? looper->getProvider() : nullptr) { }

    // fd()

protected:
    Pointer<Handler> _handler;
    Pointer<MessageQueue> _messageQueue;
};

#endif
