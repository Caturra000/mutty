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
        // return LazyEvaluate( std::bind(std::forward<Func>(functor), std::forward<Args>(args)...) );
        return LazyEvaluate ([functor, ...args2 = std::forward<Args>(args)] { functor(std::forward<decltype(args2)>(args2)...); }); // C++14 [f = std::move(f)] {...} FIXME: use MoveWrapper
    }
    void evaluate() const { _functor(); }
    void operator()() const { _functor(); }
    // void evaluateWithCheck() const { if(_functor) _functor(); } // 我寻思正常人应该没这种需求

public:
    LazyEvaluate() : _functor([]{}) { } // 提供空实现，避免check，也防止类设计时需要手动提供默认functor
    
protected:
    using Functor = std::function<void()>;
    LazyEvaluate(Functor functor) : _functor(std::move(functor)) { }
    Functor _functor;
};

// IMRPOVEMENT: 提供CountDownLazyEvaluate

#endif