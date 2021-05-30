#ifndef __MUTTY_LOOPER_H__
#define __MUTTY_LOOPER_H__
#include <bits/stdc++.h>
#include "utils/Callable.h"
#include "utils/Pointer.h"
#include "utils/Timestamp.h"
#include "utils/NonCopyable.h"
#include "base/handler/Handler.h"
#include "base/handler/Message.h"
#include "base/handler/MessageQueue.h"
#include "base/timer/Timer.h"
#include "Multiplexer.h"
namespace mutty {

class LooperHandler;
class Looper: private NonCopyable {
public:
    void loop();
    void loopAsync(std::thread &thread);
    std::thread loopAsync();

    void stop(); // thread-safe, cannot restart

    Pointer<MessageQueue> getProvider() { return &_provider; }
    Pointer<Multiplexer> getPoller() { return &_poller; }
    Pointer<Timer> getScheduler() { return &_scheduler; }

private:
    void consume(Message msg) { msg.target->handle(msg); }

private:
    bool _stop = false;
    MessageQueue _provider;
    Multiplexer _poller;
    Timer _scheduler;
};

inline void Looper::loop() {
    Timer::ResultSet tasks;
    for(MessageQueue provider; !_stop;) {
        auto timeout = _scheduler.run(tasks);
        for(auto &&task : tasks) task();
        _poller.poll(std::max(timeout, 1ms));
        /* synchronized(_provider) */ {
            std::lock_guard<std::mutex> _ {_provider.lock()};
            provider = std::move(_provider);
        }
        while(provider.hasNext()) {
            consume(provider.next());
        }
    }
}

inline void Looper::loopAsync(std::thread &thread) {
    thread = loopAsync();
}

inline std::thread Looper::loopAsync() {
    return std::thread{[this] {loop();}};
}

inline void Looper::stop() {
    _scheduler.runAt(now())
        .with([this] { _stop = true; });
}

} // mutty
#endif
