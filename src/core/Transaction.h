#ifndef __MUTTY_TRANSACTION_H__
#define __MUTTY_TRANSACTION_H__
#include <bits/stdc++.h>
#include "utils/NonCopyable.h"
#include "Looper.h"
namespace mutty {

class Transaction: private NonCopyable {
public:
    template <typename ...Args>
    Transaction(Looper *looper, Args &&...args);
    
    template <typename ...Args>
    Transaction& then(Args &&...args);

    void commit();

private:
    Callable _callable;
    Pointer<Looper> _looper;
};

template <typename ...Args>
inline Transaction::Transaction(Looper *looper, Args &&...args)
    : _looper(looper),
      _callable(Callable::make(std::forward<Args>(args)...)) {}

template <typename ...Args>
inline Transaction& Transaction::then(Args &&...args) {
    auto callable = std::move(_callable);
    _callable = Callable::make([=, callable = std::move(callable)] {
        callable();
        Callable::make(args...)();
    });
    return *this;
}

inline void Transaction::commit() {
    _looper->getScheduler()->runAt(now())
        .with(std::move(_callable));
}

} // mutty

#endif