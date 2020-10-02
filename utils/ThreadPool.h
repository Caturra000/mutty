#ifndef __UTILS_THREAD_POOL_H__
#define __UTILS_THREAD_POOL_H__
#include <bits/stdc++.h>

// using ThreadPool = std::vector<std::thread>;

// class ThreadPool: public std::vector<std::thread> {
// };

//https://blog.csdn.net/gcola007/article/details/78750220

class ThreadPool {
public:
    template <typename F, typename ...Args>
    auto post(F &&f, Args &&...args)->std::future<typename std::result_of<F(Args...)>::type>;

    ThreadPool(int size);
private:
    std::vector<std::thread> _workers;

    std::mutex _mutex;
    std::condition_variable _condition;

};

#endif