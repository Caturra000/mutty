#ifndef __TIMER_EVENT_H__
#define __TIMER_EVENT_H__
#include <bits/stdc++.h>
#include "utils/Callable.h"
#include "utils/Timestamp.h"


struct TimerEvent {
    Timestamp _when; // TODO: 修改when为时间戳类 ---FIXED
    Nanosecond _interval;
    uint64_t _atMost; // 之所以是atMost，是因为可能looper.stop()然而事件还没执行完
    uint64_t _ticket; // 不初始化也没问题
    Callable _what; // TODO 这里应该提供一个Any
    // 或者增加一个 Pointer<Handler> _who
    // atMost 和 interval可能放在Timer属性比较好，节省内存

    TimerEvent(Timestamp when, Callable what, Nanosecond interval = Millisecond::zero(), uint64_t atMost = 1)
        : _when(when), _what(std::move(what)), _interval(interval), _atMost(atMost) { }

    bool operator > (const TimerEvent &rhs) const {
        if(_when != rhs._when) return _when > rhs._when;
        return _ticket > rhs._ticket;
    }
};

#endif