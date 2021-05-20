#ifndef __ASYNC_LOOPER_CONTAINER_H__
#define __ASYNC_LOOPER_CONTAINER_H__
#include <bits/stdc++.h>
#include "Looper.h"
#include "LooperPool.h"
namespace mutty {

// 把简单的消息循环放到异步线程中
class AsyncLooperContainer {
public:
    Looper* get() { return _container.pick().get(); }

    operator Looper*() { return get(); }
private:
    LooperPool<1> _container;
};

using ClientLooper = AsyncLooperContainer;

} // mutty

#endif