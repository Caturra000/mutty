#ifndef __UTILS_OBJECT_H__
#define __UTILS_OBJECT_H__
#include <bits/stdc++.h>
// 实现C++11下的std::any
// 什么时候引入更多高版本STL依赖再去掉这个类
class Object {
public:
    Object(): _holder(nullptr) {}
    ~Object() { delete _holder; }

    template <typename T>
    Object(T &&t): _holder(new T(std::forward<T>(t))) { }

    Object(const Object &rhs)
        : _holder(rhs._holder ? rhs._holder->clone() : nullptr) {}
    Object(Object &&rhs): _holder(rhs._holder) { rhs._holder = nullptr; }
    Object & operator=(Object rhs) {
        std::swap(rhs, *this);
        return *this;
    }

    class IHolder {
    public:
        virtual IHolder* clone() = 0;
    };

    template <typename T>
    class Holder: public IHolder {
    public:
        Holder(T &&t): _t(std::forward<T>(t)) { }
        IHolder* clone() override {
            return new Holder;
        }
        
        T _t;
    };

private:
    IHolder *_holder;
};

#endif