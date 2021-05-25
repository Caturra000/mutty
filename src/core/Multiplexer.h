#ifndef __MUTTY_MULTIPLEXER_H__
#define __MUTTY_MULTIPLEXER_H__
#include <bits/stdc++.h>
#include <sys/epoll.h>
#include <poll.h>
#include <unistd.h>
#include "utils/Timestamp.h"
#include "utils/NonCopyable.h"
#include "base/context/Context.h"
#include "throws/Exceptions.h"
#include "log/Log.h"
namespace mutty {

class Multiplexer: private NonCopyable {
public:
    void poll(Nanosecond timeout);
    void update(int operation, Context *ctx);

    Multiplexer();
    ~Multiplexer() { close(_epollFd); }

private:
    void dispatchActiveContext(int count);
    void dispatch(Context *ctx, int revent);

    std::vector<epoll_event> _events;
    int _epollFd;
};

inline void Multiplexer::poll(Nanosecond timeout) {
    int count = epoll_wait(_epollFd, _events.data(), _events.size(),
                    std::chrono::duration_cast<Millisecond>(timeout).count());
    if(count < 0) throw EpollWaitException(errno);
    dispatchActiveContext(count);
    if(_events.size() == count) {
        _events.reserve(_events.size() << 1);
    }
}

inline void Multiplexer::update(int operation, Context *ctx) {
    epoll_event event {ctx->events(), ctx};
    if(epoll_ctl(_epollFd, operation, ctx->fd(), &event)) {
        throw EpollControlException(errno);
    }
}

inline Multiplexer::Multiplexer(): _epollFd(epoll_create1(EPOLL_CLOEXEC)), _events(16) {
    if(_epollFd < 0) throw EpollCreateException(errno);
}

inline void Multiplexer::dispatchActiveContext(int count) {
    // if(count > 0) {
    //     MUTTY_LOG_DEBUG("multiplexer dispatches", count, "active context(s)");
    // }
    for(int i = 0; i < count; ++i) {
        auto ctx = static_cast<Context*>(_events[i].data.ptr);
        auto revent = _events[i].events;
        dispatch(ctx, revent);
    }
}

inline void Multiplexer::dispatch(Context *ctx, int revent) {
    // MUTTY_LOG_DEBUG("multiplexer dispatch: context_fd =", ctx->fd(), "revent_mask = ", revent);
    if((revent & POLLHUP) && !(revent & POLLIN)) {
        ctx->sendCloseMessage();
    }
    if(revent & (POLLERR | POLLNVAL)) {
        ctx->sendErrorMessage();
    }
    if(revent & (POLLIN | POLLPRI | POLLRDHUP)) {
        ctx->sendReadMessage();
    }
    if(revent & POLLOUT) {
        ctx->sendWriteMessage();
    }
}

} // mutty
#endif