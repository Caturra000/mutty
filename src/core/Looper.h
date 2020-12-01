#ifndef __LOOPER_H__
#define __LOOPER_H__
#include <bits/stdc++.h>
#include "utils/Callable.h"
#include "utils/Pointer.h"
#include "utils/Timestamp.h"
#include "base/handler/Handler.h"
#include "base/handler/Message.h"
#include "base/handler/MessageQueue.h"
#include "base/timer/Timer.h"
#include "Multiplexer.h"

class Looper {
public:
    void loop() {
        if(_threadId != std::this_thread::get_id()) return;
        std::vector<const Callable*> callables;
        for(MessageQueue provider; !_stop || onStop(); ) {
            auto timeout = _scheduler.run(callables);
            _poller.poll(std::max(std::chrono::duration_cast<Millisecond>(timeout), 1ms)); // TODO Config
            // IMRPOVEMENT: 是否可根据一些条件来得知provider没有数据，节省不必要的上锁？
            {
                std::lock_guard<std::mutex> _ {_provider.lock()};
                provider = std::move(_provider); // move并不移动原有的锁
            }
            for(auto &&it : callables) {
                it->call();
            }
            while(provider.hasNext()) {
                consume(provider.next());
            }
        }
    }



    void stop() { _stop = true; }
    bool onStop() { return _provider.hasNextUnlock(); } // override

    Pointer<MessageQueue> getProvider() { return &_provider; }
    Pointer<Multiplexer> getPoller() { return &_poller; }
    Pointer<Timer> getScheduler() { return &_scheduler; }
    
private:
    // In Loop Thread
    void consume(Message msg) {
        msg.target->handle(msg);
    }

private:
    bool _stop = false;
    std::thread::id _threadId = std::this_thread::get_id();
    MessageQueue _provider; // provider不会区分不同类型的消息，总是对应于同一个Looper
    Multiplexer _poller;
    Timer _scheduler;
};
#endif
