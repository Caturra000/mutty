#ifndef __LOOPER_H__
#define __LOOPER_H__
#include <bits/stdc++.h>
#include "utils/LazyEvaluate.h"
#include "utils/Pointer.h"
#include "utils/Timestamp.h"
#include "Handler.h"
#include "Message.h"
#include "MessageQueue.h"
#include "Multiplexer.h"
#include "TimerEvent.h"
#include "TimerHelper.h"
// 处理一个Channel中的所有IO操作
// 一般一个Looper可以对付多个Channel
class Context;
class Looper {
    friend Context;
public:
    // static
    void loop() {
        if(_threadId != std::this_thread::get_id()) return;
        // if(_quit) return;
        //_timer.run();

        for(; !_quit; ) {
            // TODO: poll---->append msg到mq中
            _timer.run();  
            // 考虑到MPSC模型，用move来避免处理消息SC对MP的阻塞
            // MP之间没有重量级操作，用mutex的自旋优化即可
            MessageQueue provider;
            {
                std::lock_guard<std::mutex> _ {_provider.queueLock()};
                provider = std::move(_provider); // move并不移动原有的锁
            }
            //auto provider = std::move(_provider);
            while(provider.hasNext()) {
                consume(provider.next());
            }
        }
        // me = myLooper();
        // mq = me.mq;
        // for(;;)
        //     msg = mq.next();
        //     msg.target.dispatch(msg);
    }

    void consume(Message msg) {
        msg.target->handle(msg);
        // 为了以后只需继承handler就能修改消息处理机制，实现还是不要放到Looper上
        // auto consumer = msg.target;
        // switch(msg._what) {
        //     case 0:
        //         consumer->onConnected();
        //     break;
        //     default:
        //     break;
        // }
    }

    void stop() { _quit = true; }


    // usage:
    // looper.runAfter(2s).per(2s).atMost(3).with(func, arg0, arg1);
    // looper.runEvery(500ms).at(now()+1s).with([]{std::cerr<<"b";});

    // IMPROVEMENT: 一个显然的优化是如果时间就是现在/过去 & 只发生一次，那么直接调用即可，省去插入heap的成本

    TimerHelper runAt(Timestamp when) { return TimerHelper{when, Millisecond::zero(), 1, &_timer}; }
    TimerHelper runAfter(Nanosecond interval) { return TimerHelper{nowAfter(interval), Millisecond::zero(), 1, &_timer}; }
    TimerHelper runEvery(Nanosecond interval) { return TimerHelper{now(), interval, std::numeric_limits<uint64_t>::max(), &_timer}; }

private:
    bool _quit = false;
    std::thread::id _threadId = std::this_thread::get_id();
    MessageQueue _provider; // MessageQueue对应一个Looper,但是Looper是存在并发的，因此需要保证线程安全
    Multiplexer _multiplexer;
    Timer _timer;
};
// 一个特殊的地方是这里MQ提供的是msg，而不是一条连接（Channel）
// 在这里Context才是代表一条连接（的上下文）
#endif
