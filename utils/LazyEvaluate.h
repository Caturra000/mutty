#ifndef __UTILS_LAZY_EVALUATE_H__
#define __UTILS_LAZY_EVALUATE_H__
#include <bits/stdc++.h>
#include "Noncopyable.h"

// FIXME: 目前对于lambda无法提供有参数的支持[](必须空) {}
// 目前的问题是即便在lazy阶段也会出错，因为functor本身是一个function<void()>
// 用template好像确实是最简单的改法，对于lazy和evalute都能零成本提供接口，只是这样的话，普通的函数体还能不能放在一块？  
// 到头来还是变成了std::bind和占位符的设计，果然STL写的是通用的
// 不如尝试用std::bind再一次倒推出这种封装？这次改为封装占位符
// 更加通用的话可以造一个is_lambda轮子：https://stackoverflow.com/questions/4661875/is-a-c-is-lambda-trait-purely-implemented-as-a-library-impossible
// 通过is_lambda和编译时判断是否可构造为function<void()>来另开一条分支

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