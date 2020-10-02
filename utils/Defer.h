#ifndef __UTILS_DEFER_H
#define __UTILS_DEFER_H
#include <bits/stdc++.h>
class Defer {
private:
    using Defer_ = std::shared_ptr<Defer>;
    Defer_ defer_;
public:
    template <typename T, typename ...Args>
    Defer(T &&callable, Args &&...args)
        : defer_(nullptr, [=](Defer*) { callable(std::move(args)...); }) { }
};
#endif