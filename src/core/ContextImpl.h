#ifndef __MUTTY_CONTEXT_IMPL_H__
#define __MUTTY_CONTEXT_IMPL_H__
#include <poll.h>
#include <bits/stdc++.h>
#include "utils/Pointer.h"
#include "base/context/Context.h"
#include "base/handler/Handler.h"
#include "base/handler/MessageQueue.h"
#include "base/handler/Message.h"
#include "core/Looper.h"
#include "core/Multiplexer.h"
#include "log/Log.h"
namespace mutty {

class ContextImpl: public Context {
public:
    template <typename ...Args>
    void async(Args &&...args);

    uint32_t events() const override { return _events; }
    bool readEnabled() override { return _events & EVENT_READ; }
    bool writeEnabled() override { return _events & EVENT_WRITE; }

protected:
    void sendMessage(int what) { _messageQueue->post({_handler.get(), what}); }
    void sendMessage(int what, int uFlag) { _messageQueue->post({_handler.get(), what, uFlag}); }
    void sendMessageWithData(int what, void *data);
    void sendMessageWithBinaryData(int what, int size, void *data);

    void sendReadMessage()  override { sendMessage(MSG_POLL_READ);  }
    void sendWriteMessage() override { sendMessage(MSG_POLL_WRITE); }
    void sendErrorMessage() override { sendMessage(MSG_POLL_ERROR); }
    void sendCloseMessage() override { sendMessage(MSG_POLL_CLOSE); }

    void updateState() override;

    void enableRead() override;
    void enableWrite() override;
    void disableRead() override;
    void disableWrite() override;

    // debug

    const char* stateToString() const;
    const char* eventToString() const;

    ContextImpl(Handler *handler = nullptr, Looper *looper = nullptr);

    // fd()

protected:
    Pointer<Handler> _handler;
    Pointer<Looper> _looper;
    Pointer<MessageQueue> _messageQueue;
    Pointer<Multiplexer> _multiplexer;

    int _state;
    uint32_t _events;

    constexpr static int STATE_NEW = 0;
    constexpr static int STATE_ADDED = 1;
    constexpr static int STATE_DELETED = 2;

// for events

    constexpr static uint32_t EVENT_NONE = 0;
    constexpr static uint32_t EVENT_READ = POLL_IN | POLL_PRI;
    constexpr static uint32_t EVENT_WRITE = POLL_OUT;
};

template <typename ...Args>
inline void ContextImpl::async(Args &&...args) {
    _looper->getScheduler()->runAt(now())
        .with(Callable::make(std::forward<Args>(args)...));
}

inline void ContextImpl::sendMessageWithData(int what, void *data) {
    _messageQueue->post( Message {
        .target = _handler.get(),
        .what = what,
        .any = data
    });
}

inline void ContextImpl::sendMessageWithBinaryData(int what, int size, void *data) {
    _messageQueue->post({_handler.get(), what, size, data});
}

inline void ContextImpl::updateState() {
    MUTTY_LOG_DEBUG("try to update context state,", "original state =", stateToString(),_state, "fd =", fd());
    if(_state == STATE_NEW || _state == STATE_DELETED) {
        _state = STATE_ADDED;
        _multiplexer->update(EPOLL_CTL_ADD, this);
        MUTTY_LOG_DEBUG("context updated, added to epoll", "state = ADDING", "fd =", fd());
    } else { // STATE_ADDED
        if(_events == EVENT_NONE) {
            _multiplexer->update(EPOLL_CTL_DEL, this);
            _state = STATE_DELETED;
            MUTTY_LOG_DEBUG("context updated, deleted from epoll", "state = DELETED", "fd =", fd());
        } else {
            _multiplexer->update(EPOLL_CTL_MOD, this);
            MUTTY_LOG_DEBUG("context updated, modified", "fd =", fd());
        }
    }
}

inline const char* ContextImpl::eventToString() const {
    switch(_events) {
        case EVENT_NONE: return "NONE";
        case EVENT_READ: return "READ";
        case EVENT_WRITE: return "WRITE";
        case EVENT_READ | EVENT_WRITE: return "READ_WRITE";
        default: return "?";
    }
}

inline void ContextImpl::enableRead() {
    if(!(_events & EVENT_READ)) {
        _events |= EVENT_READ;
        MUTTY_LOG_DEBUG("enable read event", "fd =", fd(), "event =", eventToString());
        updateState();
    }
}

inline void ContextImpl::enableWrite() {
    if(!(_events & EVENT_WRITE)) {
        _events |= EVENT_WRITE;
        MUTTY_LOG_DEBUG("enable write event", "fd =", fd(), "event =", eventToString());
        updateState();
    }
}

inline void ContextImpl::disableRead() {
    if(_events & EVENT_READ) {
        _events &= ~EVENT_READ;
        MUTTY_LOG_DEBUG("disable read event", "fd =", fd(), "event =", eventToString());
        updateState();
    }
}

inline void ContextImpl::disableWrite() {
    if(_events & EVENT_WRITE) {
        _events &= ~EVENT_WRITE;
        MUTTY_LOG_DEBUG("disable write event", "fd =", fd(), "event =", eventToString());
        updateState();
    }
}

inline const char* ContextImpl::stateToString() const {
    switch(_state) {
        case STATE_NEW: return "NEW";
        case STATE_ADDED: return "ADDED";
        case STATE_DELETED: return "DELETED";
        default: return "?";
    }
}

inline ContextImpl::ContextImpl(Handler *handler, Looper *looper)
    : _handler(handler),
      _looper(looper),
      _messageQueue(looper ? looper->getProvider() : nullptr),
      _multiplexer(looper ? looper->getPoller() : nullptr),
      _state(STATE_NEW), _events(EVENT_NONE) {}
} // mutty
#endif
