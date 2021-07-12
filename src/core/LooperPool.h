#ifndef __MUTTY_LOOPER_POOL_H__
#define __MUTTY_LOOPER_POOL_H__
#include <bits/stdc++.h>
#include "utils/Algorithms.h"
#include "utils/ThreadPool.h"
#include "utils/Compat.h"
#include "utils/NonCopyable.h"
#include "log/Log.h"
#include "Looper.h"
namespace mutty {

// TODO remove template, use std::thread::hardware_concurrency() by default
//     std::array<unique_ptr> -> std::vector && remove unique_ptr
//     random & N-1 -> % N
template <size_t N>
class LooperPool: private NonCopyable {
public:
    std::unique_ptr<Looper>& pick();
    std::unique_ptr<Looper>& pick(size_t index) { return _pool[index]; }

    LooperPool();

private:
    std::array<std::unique_ptr<Looper>, N> _pool;
    ThreadPool _threads;
};

template <size_t N>
inline std::unique_ptr<Looper>& LooperPool<N>::pick() {
    size_t index = random<size_t>() & N-1;
    MUTTY_LOG_DEBUG("LooperPool: pick index =", index);
    return _pool[index];
}

template <size_t N>
inline LooperPool<N>::LooperPool(): _threads(N) {
    constexpr size_t mayBeZero = N & N-1;
    static_assert(mayBeZero == 0, "N must be power of two.");
    for(auto &looper : _pool) {
        looper = cpp11::make_unique<Looper>();
        _threads.execute([&] {looper->loop();});
    }
}

} // mutty
#endif