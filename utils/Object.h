#ifndef __UTILS_OBJECT_H__
#define __UTILS_OBJECT_H__
#include <bits/stdc++.h>
// 实现C++11下的std::any
// 什么时候引入更多高版本STL依赖再去掉这个类
class Object {
public:
    Object(): _content(nullptr) {}
    ~Object() { delete _content; }

    template <typename ValueType>
    Object(const ValueType &content): _content(new Holder<ValueType>(content)) { }

    Object(const Object &rhs)
        : _content(rhs._content ? rhs._content->clone() : nullptr) {}
    Object(Object &&rhs): _content(rhs._content) { rhs._content = nullptr; }
    Object& operator=(Object rhs) {
        std::swap(rhs, *this);
        return *this;
    }


    class IHolder {
    public:
        virtual IHolder* clone() = 0;
    };

    template <typename ValueType>
    class Holder: public IHolder {
    public:
        Holder(const ValueType &value): _value(value) { }
        IHolder* clone() override {
            return new Holder(_value);
        }
        ValueType _value;
    };

private:
    IHolder *_content;
};

#endif