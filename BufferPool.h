#ifndef __BUFFER_POOL_H__
#define __BUFFER_POOL_H__
#include <bits/stdc++.h>
#include "Buffer.h"
#include "utils/LazyEvaluate.h"
#include "utils/Algorithms.h"

// using CachedBuffer = std::shared_ptr<Buffer>; // 不需要引用计数

class CachedBuffer: public Buffer {
public:
    using Buffer::Buffer;
    CachedBuffer(int size, std::function<void(CachedBuffer&)> destructor)
        : Buffer(size), _destructor(std::move(destructor)) { }

    CachedBuffer(CachedBuffer&&) = default;
    CachedBuffer(const CachedBuffer&) = default;
    CachedBuffer& operator=(CachedBuffer&&) = default;
    CachedBuffer& operator=(const CachedBuffer&) = default;
    ~CachedBuffer() {
        if(_destructor) _destructor(*this);
    }
private:
    std::function<void(CachedBuffer&)> _destructor;
};





class BufferPool {
public:
    CachedBuffer obtain(int sizeHint) { // FIXME: 不允许有0大小的buffer
        int n = findCeilOfPowerOfTwo(sizeHint);
        if(!_pool[n].empty()) {
            auto cachedBuffer = std::move(_pool[n].back());
            _pool[n].pop_back();
            return cachedBuffer;
        }
        return CachedBuffer(roundToPowerOfTwo(sizeHint), [this](CachedBuffer &buffer) {
            int n = findCeilOfPowerOfTwo(buffer.size()); // 下取整
            buffer.clear();
            _pool[n].emplace_back(std::move(buffer));
        });
    }


private:
    std::array<std::vector<CachedBuffer>, 32> _pool;



};
#endif