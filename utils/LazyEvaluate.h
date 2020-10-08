#ifndef __UTILS_LAZY_EVALUATE_H__
#define __UTILS_LAZY_EVALUATE_H__
#include <bits/stdc++.h>
#include "Noncopyable.h"


// @brief: a wrapper of std::function and std::bind, mainly used for callback
// @usage:
//     auto callback = LazyEvaluate::lazy(functor, args);
//     // ....
//     callback.evaluate();
class LazyEvaluate {
public:
    // smart pointer wrapper
    // IMPROVEMENT: 通过template实现默认返回unique，但是可以通过<shared_ptr>返回一个shared_ptr
    template <typename Func, typename ...Args>
    static std::unique_ptr<LazyEvaluate> make(Func &&functor, Args &&...args) {
        return std::make_unique<LazyEvaluate>(
            lazy(std::forward<Func>(functor), std::forward<Args>(args)...));
    }

    template <typename Func, typename ...Args>
    static LazyEvaluate lazy(Func &&functor, Args &&...args) {
        return LazyEvaluate( std::bind(std::forward<Func>(functor), std::forward<Args>(args)...) );
    }
    void evaluate() const { _functor(); }
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