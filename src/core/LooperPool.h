#ifndef __MUTTY_LOOPER_POOL_H__
#define __MUTTY_LOOPER_POOL_H__
#include <bits/stdc++.h>
#include "utils/Algorithms.h"
#include "utils/ThreadPool.h"
#include "utils/Compat.h"
#include "Looper.h"
namespace mutty {

template <size_t N>
class LooperPool {
public:
    std::unique_ptr<Looper>& pick() { return _pool[random<size_t>() & N-1]; }

    LooperPool();

private:
    std::array<std::unique_ptr<Looper>, N> _pool;
    ThreadPool _threads;
};

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