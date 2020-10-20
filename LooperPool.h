#ifndef __LOOPER_POOL_H__
#define __LOOPER_POOL_H__
#include <bits/stdc++.h>
#include "utils/Algorithms.h"
#include "Looper.h"

// 具体用途是每次accept都分配到固定大小的Looper池中
template <size_t N>
class LooperPool {
public:
    std::unique_ptr<Looper>& pick() {
        auto index = random<size_t>() & N-1;
        // TODO if nullptr....
        return _pool[index];
    }

    LooperPool() {
        constexpr size_t mayBeZero = N & N-1;
        static_assert(mayBeZero == 0, "N must be power of two.");
    }


private:
    std::array<std::unique_ptr<Looper>, N> _pool;
};

// using SimpleLooperPool = 
#endif