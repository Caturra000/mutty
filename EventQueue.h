#ifndef __EVENT_QUEUE_H__
#define __EVENT_QUEUE_H__
#include <bits/stdc++.h>
#include "utils/LazyEvaluate.h"

// 后期实现上需要再添加
class EventQueue {
public:
    
private:
    std::queue<LazyEvaluate> _queue;
    std::mutex _mutex;
    std::thread::id _threadId = std::this_thread::get_id();
};
#endif