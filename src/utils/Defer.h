#ifndef __MUTTY_DEFER_H__
#define __MUTTY_DEFER_H__
#include <bits/stdc++.h>
namespace mutty {

class Defer: std::__shared_ptr<Defer, std::_S_single> {
public:
    template <typename T, typename ...Args>
    Defer(T &&func, Args &&...args)
        : std::__shared_ptr<Defer, std::_S_single>
          (nullptr, [=](Defer*) { func(std::move(args)...); }) {}
};

} // mutty
#endif