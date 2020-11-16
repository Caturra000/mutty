#ifndef __LOOPER_H__
#define __LOOPER_H__
#include <bits/stdc++.h>
#include "utils/LazyEvaluate.h"
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
        
        for(MessageQueue provider; !_quit; ) {
            auto timeout = _timer.run();
            _poller.poll(std::max(std::chrono::duration_cast<Millisecond>(timeout), 1ms)); // TODO Config
            // IMRPOVEMENT: 是否可根据一些条件来得知provider没有数据，节省不必要的上锁？
            {
                std::lock_guard<std::mutex> _ {_provider.queueLock()};
                provider = std::move(_provider); // move并不移动原有的锁
            }
            while(provider.hasNext()) {
                consume(provider.next());
            }
        }
    }



    void stop() { _quit = true; }

    Pointer<MessageQueue> getProvider() { return &_provider; }
    Pointer<Multiplexer> getPoller() { return &_poller; }
    Pointer<Timer> getTimer() { return &_timer; }
    
private:
    // In Loop Thread
    void consume(Message msg) {
        msg.target->handle(msg);
    }

private:
    bool _quit = false;
    std::thread::id _threadId = std::this_thread::get_id();
    MessageQueue _provider; // provider不会区分不同类型的消息，总是对应于同一个Looper
    Multiplexer _poller;
    Timer _timer;
};
#endif
