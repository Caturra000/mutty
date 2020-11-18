#ifndef __UTILS_LAZY_EVALUATE_H__
#define __UTILS_LAZY_EVALUATE_H__
#include <bits/stdc++.h>

// @brief: a wrapper of std::function, mainly used for callback
// @usage:
//     auto callback = LazyEvaluate::lazy(functor, args);
//     // ....
//     callback.evaluate();
class LazyEvaluate {
public:
    template <typename Func, typename ...Args>
    static LazyEvaluate lazy(Func &&functor, Args &&...args) {
#if __cplusplus >= 201402L
        return LazyEvaluate([=, f = std::forward<Func>(functor)]{f(args...);});
#else
        return LazyEvaluate([=]{functor(args...);});
#endif
    }
    void evaluate() const { _functor(); }
    void operator()() const { _functor(); }

public:
    LazyEvaluate() : _functor([]{}) { }
    
protected:
    using Functor = std::function<void()>;
    LazyEvaluate(Functor functor) : _functor(std::move(functor)) { }
    Functor _functor;
};

// IMRPOVEMENT: 提供CountDownLazyEvaluate

#endif