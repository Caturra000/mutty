#ifndef __TIMER_HANDLER_H__
#define __TIMER_HANDLER_H__
#include <bits/stdc++.h>
#include "Handler.h"
#include "Message.h"
#include "LazyEvaluate.h"
class TimerHandler: public Handler {
public:
    HANDLER_MSG_DEFINE(MSG_TIMER_TIMED); // 定时器到时间了

    void handle(Message msg) override {
        switch(msg._what) {
            case MSG_TIMER_TIMED:
                if(!_pending.empty()) {
                    _pending.front().evaluate();
                    _pending.pop_front();
                }
            break;
            default:
            break;
        }
    }
private:
    std::deque<LazyEvaluate> _pending;

};
#endif