#ifndef __TIMER_H__
#define __TIMER_H__
#include <bits/stdc++.h>
#include "TimerEvent.h"
#include "utils/Timestamp.h"
#include "utils/Defer.h"
#include "utils/Random.h"



class Timer {
    using EventHeap = std::priority_queue<
            TimerEvent, std::vector<TimerEvent>, std::greater<TimerEvent>>;    
public:
    Timer() = default;
    Timer(std::initializer_list<TimerEvent> eventList): Timer() {
        for(auto &e : eventList) {
            _container.push(std::move(const_cast<TimerEvent&>(e))); //FIXME:可能需要改接口为append
        }
    }
    // 虽然接口丑了点，但是可以匹配TimerEvent任意构造函数，相比hardcode/多个重载都要方便点
    template <typename ...EventConstructorArgs>
    Timer& append(EventConstructorArgs &&...args) {
        std::lock_guard<std::mutex> _{_mutex};
        _container.push(TimerEvent(std::forward<EventConstructorArgs>(args)...));
        return *this;
    }

    // 不会阻塞，只会执行到不符合条件的时间就返回
    void run() {
        std::lock_guard<std::mutex> _ {_mutex};
        if(_container.empty()) return;
        std::vector<TimerEvent> newbee;
        Timestamp current = now();
        while(!_container.empty()) {
            auto &e = _container.top();
            if(e._when > current) break;
            Defer _ {[this] { _container.pop(); }};
            if(e._atMost > 0) e._what.evaluate();
            if(e._atMost > 1) {
                newbee.push_back(e);
                auto &b = newbee.back();
                b._atMost--;
                b._when += b._interval;
                // 解决资源饥饿，每8次重新调度
                if(!(b._atMost & 7)) b._ticket += random();
            }
        }
        for(auto &e : newbee) _container.push(std::move(e));
    }

private:
    EventHeap _container;
    bool _running {false};
    std::mutex _mutex;
};

#endif