#ifndef __UTILS_THREAD_POOL_H__
#define __UTILS_THREAD_POOL_H__
#include <bits/stdc++.h>
#include "utils/LazyEvaluate.h"
// FIXED
class ThreadPool {
public:
    explicit ThreadPool(int size) {
        while(size--) run();
    }

    ~ThreadPool() {
        if(_data) {
            {
                std::lock_guard<std::mutex> _ { _data->_mutex};
                _data->_stop = true;
            }
            _data->_condition.notify_all();
        }
    }

    template <typename Func, typename ...Args>
    void execute(Func &&functor, Args &&...args) {
        {
            std::lock_guard<std::mutex> _ { _data->_mutex};
            _data->_tasks.emplace(LazyEvaluate::lazy(
                std::forward<Func>(functor), std::forward<Args>(args)...));
        }
        _data->_condition.notify_one();
    }

private:

    void run() {
        std::thread { [this] {
            auto data = _data; // lifecycle
            std::unique_lock<std::mutex> lock {data->_mutex};
            for(;;) {
                if(!data->_tasks.empty()) {
                    auto task = std::move(data->_tasks.front());
                    data->_tasks.pop();
                    lock.unlock();
                    task.evaluate();
                    lock.lock();
                } else if(data->_stop) {
                    break;
                } else {
                    data->_condition.wait(lock);
                }
            }
        }}.detach();
    }

    struct Data {
        std::mutex _mutex;
        std::condition_variable _condition;
        bool _stop {false};
        std::queue<LazyEvaluate> _tasks;
    };
    std::shared_ptr<Data> _data {std::make_shared<Data>()};

    


};

#endif