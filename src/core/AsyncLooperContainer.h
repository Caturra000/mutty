#ifndef __MUTTY_ASYNC_LOOPER_CONTAINER_H__
#define __MUTTY_ASYNC_LOOPER_CONTAINER_H__
#include <bits/stdc++.h>
#include "Looper.h"
#include "LooperPool.h"
namespace mutty {

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