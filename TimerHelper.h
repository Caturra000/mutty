#ifndef __TIMER_HELPER_H__
#define __TIMER_HELPER_H__
#include <bits/stdc++.h>
#include "TimerEvent.h"
#include "Timer.h"
#include "utils/Timestamp.h"
#include "utils/Pointer.h"
// 主要的作用是使得Looper更加易用，并且避开了TimerEvent必须使用LazyEvaluate显式构造的毛病，提高愉悦度(真的）
// FIXME: 这里稍微hardcode了点，效率不见得高
// usage:
// looper.runAt(10s)
//         .with(func, arg0, arg1, arg2...)
//         .with(func2, arg0, arg1...);
struct TimerHelper {
    Timestamp _when;
    Nanosecond _interval;
    uint64_t _atMost;
    Pointer<Timer> _thisTimer;

    template <typename ...Args>
    TimerHelper& with(Args &&...args) { 
        _thisTimer->append(_when, LazyEvaluate::lazy(std::forward<Args>(args)...), _interval, _atMost); 
        return *this;    // 不暴露Timer接口
    }

    //IMPROVEMENT:增加更多特定
    // 实现更多组合:
    // looper.runAfter(2s).per(2s).atMost(3).with([]{std::cerr<<"a";});
    // looper.runEvery(500ms).at(now()+1s).with([]{std::cerr<<"b";});


    TimerHelper& per(Nanosecond interval) {
        _interval = interval;
        return *this;
    }
    TimerHelper& atMost(uint64_t times) {
        _atMost = times;
        return *this;
    }
    TimerHelper at(Timestamp when) {
        _when = when;
        return *this;
    }
};
#endif