#ifndef __TRANSACTION_H__
#define __TRANSACTION_H__
#include <bits/stdc++.h>
#include "Looper.h"
namespace mutty {

// simple wrapper
// 用于合并提交，可处理分离的逻辑，并保证线程安全
class Transaction {
public:
    template <typename ...Args>
    Transaction(Looper *looper, Args &&...args)
        : _looper(looper),
            _callable(Callable::make(std::forward<Args>(args)...)) {}
    
    template <typename ...Args>
    Transaction& then(Args &&...args) {
        auto callable = std::move(_callable);
        _callable = Callable::make([=, callable = std::move(callable)] {
            callable();
            Callable::make(args...)();
        });
        return *this;
    }
    void commit() {
        _looper->getScheduler()->runAt(now())
            .with(std::move(_callable));
    }
private:
    Callable _callable;
    Pointer<Looper> _looper;
};

} // mutty

#endif