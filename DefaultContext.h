#ifndef __DEFAULT_CONTEXT_H__
#define __DEFAULT_CONTEXT_H__
#include <bits/stdc++.h>
#include "Context.h"
#include "MessageQueue.h"
#include "Message.h"
#include "utils/Pointer.h"

class Handler;

// 用于简单实现的包装类，如果是相对复杂的实现，应该直接继承Context，并且明确Handler是哪种类型
class DefaultContext: public Context {
public:
    CONTEXT_MSG_DEFINE(MSG_READ);
    CONTEXT_MSG_DEFINE(MSG_WRITE);
    CONTEXT_MSG_DEFINE(MSG_ERROR);
    CONTEXT_MSG_DEFINE(MSG_CLOSE);


    DefaultContext(Handler *handler = nullptr, MessageQueue *messageQueue = nullptr)
        : _handler(handler),
          _messageQueue(messageQueue) { }

    void sendReadMessage() override { if(_messageQueue) _messageQueue->post({_handler, MSG_READ}); }
    void sendWriteMessage() override { if(_messageQueue) _messageQueue->post({_handler, MSG_WRITE}); }
    void sendErrorMessage() override { if(_messageQueue) _messageQueue->post({_handler, MSG_ERROR}); }
    void sendCloseMessage() override { if(_messageQueue) _messageQueue->post({_handler, MSG_CLOSE}); }

    // fd()

protected:
    Pointer<Handler> _handler;
    Pointer<MessageQueue> _messageQueue;
};

#endif
