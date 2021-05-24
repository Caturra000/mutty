#ifndef __MUTTY_TIMER_H__
#define __MUTTY_TIMER_H__
#include <bits/stdc++.h>
#include "utils/Timestamp.h"
#include "utils/Defer.h"
#include "utils/Algorithms.h"
#include "utils/Pointer.h"
#include "utils/NonCopyable.h"
#include "TimerEvent.h"
namespace mutty {

class Timer: private NonCopyable {
public:
    using EventHeap = std::priority_queue<
            TimerEvent, std::vector<TimerEvent>, std::greater<TimerEvent>>;
    using ResultSet = std::vector<TimerEvent>;
    Timer() = default;
    Timer(std::initializer_list<TimerEvent> eventList);

    template <typename ...EventConstructorArgs>
    Timer& append(EventConstructorArgs &&...args);

    Millisecond run();
    Millisecond run(ResultSet &tasks);

    enum Urgency {
        MINOR = std::numeric_limits<uint64_t>::max(),
        MAJOR = MINOR >> 30,
        CRITICAL = MAJOR >> 30,
    };

    struct TimerHelper {
        Timestamp _when;
        Nanosecond _interval;
        uint64_t _atMost;
        Timer *_thisTimer;

        template <typename ...Args>
        TimerHelper& with(Args &&...args);
        TimerHelper& per(Nanosecond interval);
        TimerHelper& atMost(uint64_t times);
        TimerHelper& at(Timestamp when);
        TimerHelper& priority(Urgency urgency);
    };

    // usage:
    // runAfter(2s).per(2s).atMost(3).with(func, arg0, arg1);
    // runEvery(500ms).at(now()+1s).with([]{std::cerr<<"b";});

    TimerHelper runAt(Timestamp when) 
        { return TimerHelper{when, Millisecond::zero(), 1, this}; }
    TimerHelper runAfter(Nanosecond interval) 
        { return TimerHelper{nowAfter(interval), Millisecond::zero(), 1, this}; }
    TimerHelper runEvery(Nanosecond interval) 
        { return TimerHelper{now(), interval, std::numeric_limits<uint64_t>::max(), this}; }

// Builder End

private:
    EventHeap _container;
    bool _running {false};
    std::mutex _mutex;
    ResultSet _reenterables;
};

inline Timer::Timer(std::initializer_list<TimerEvent> eventList): Timer() {
    for(auto &e : eventList) {
        _container.push(std::move(const_cast<TimerEvent&>(e)));
    }
}

template <typename ...EventConstructorArgs>
inline Timer& Timer::append(EventConstructorArgs &&...args) {
    std::lock_guard<std::mutex> _{_mutex};
    _container.push(TimerEvent(std::forward<EventConstructorArgs>(args)...));
    return *this;
}

inline Millisecond Timer::run() {
    std::lock_guard<std::mutex> _ {_mutex};
    if(_container.empty()) return Millisecond::zero();
    Timestamp current = now();
    while(!_container.empty()) {
        auto &e = _container.top();
        if(e._when > current) break;
        Defer _ {[this] { _container.pop(); }};
        if(e._atMost > 0) e._what.call();
        if(e._atMost > 1) {
            _reenterables.push_back(std::move(const_cast<TimerEvent&>(e)));
            auto &b = _reenterables.back();
            b.next();
        }
    }
    for(auto &e : _reenterables) _container.push(std::move(e));
    _reenterables.clear();
    if(_container.empty()) return Millisecond::zero();
    return std::chrono::duration_cast<Millisecond>(_container.top()._when - now());
}

inline Millisecond Timer::run(ResultSet &tasks) {
    std::lock_guard<std::mutex> _ {_mutex};
    Timestamp current = now();
    for(size_t n = tasks.size(); n; --n) {
        auto &task = tasks.back();
        if(task._atMost > 1) {
            task.next();
            if(task._when > current) {
                _container.emplace(std::move(task));
            } else {
                _reenterables.emplace_back(std::move(task));
            }
        }
        tasks.pop_back();
    }
    _reenterables.swap(tasks);
    if(_container.empty()) return Millisecond::zero();
    while(!_container.empty()) {
        auto &task = _container.top();
        if(task._when > current) break;
        tasks.emplace_back(std::move(const_cast<TimerEvent&>(task)));
        _container.pop();
    }
    if(_container.empty()) return Millisecond::zero();
    return std::chrono::duration_cast<Millisecond>(_container.top()._when - now());
}

template <typename ...Args>
inline Timer::TimerHelper& Timer::TimerHelper::with(Args &&...args) { 
    _thisTimer->append(_when, 
        Callable::make(std::forward<Args>(args)...), 
        _interval, _atMost); 
    return *this;
}

inline Timer::TimerHelper& Timer::TimerHelper::per(Nanosecond interval) {
    _interval = interval;
    return *this;
}

inline Timer::TimerHelper& Timer::TimerHelper::atMost(uint64_t times) {
    _atMost = times;
    return *this;
}

inline Timer::TimerHelper& Timer::TimerHelper::at(Timestamp when) {
    _when = when;
    return *this;
}

inline Timer::TimerHelper& Timer::TimerHelper::priority(Urgency urgency) {
    _atMost = urgency;
    return *this;
}

} // mutty
#endif