#ifndef __UTILS_DEFER_H
#define __UTILS_DEFER_H
#include <bits/stdc++.h>
namespace mutty {

class Defer {
private:
    using DeferImpl = std::__shared_ptr<Defer, std::_S_single>;
    DeferImpl _defer;
public:
    template <typename T, typename ...Args>
    Defer(T &&callable, Args &&...args)
        : _defer(nullptr, [=](Defer*) { callable(std::move(args)...); }) { }
};

} // mutty
#endif