#ifndef __MUTTY_POINTER_H__
#define __MUTTY_POINTER_H__
#include <bits/stdc++.h>
namespace mutty {

template <typename T>
class Pointer {
public:
    Pointer(T* ptr = nullptr): _ptr(ptr) { }
    Pointer(const Pointer &rhs): _ptr(rhs._ptr) { }
    Pointer(Pointer &&rhs): _ptr(rhs._ptr) { rhs._ptr = nullptr; }
    Pointer operator++() { return ++_ptr; }
    Pointer operator++(int) { return _ptr++; }
    Pointer& operator=(Pointer rhs) {
        std::swap(_ptr, rhs._ptr);
        return *this;
    }
    T& operator*() { return *_ptr; }
    T* operator->() { return _ptr; }
    explicit operator bool() { return _ptr; }
    ~Pointer() { }

    T* get() { return _ptr; } // 兼容

    template <typename Base>
    Base* castTo() { 
        static_assert(std::is_base_of<Base, T>::value, "can only cast to Pointer<Base>/Base*");
        return _ptr;
    }

protected:
    T* _ptr;
};



/*
    Pointer<int> p = new int[10] {1, 2, 3, 4, 5};
    std::cout<< *p++ << std::endl;
    std::cout << *p << std::endl;

    Pointer<std::vector<int>> pv = new std::vector<int>{1,2,3,4,5}; // 如果是std::vector<int>*，最后两个输出是同一地址
    std::cout << &(*pv) << std::endl;
    std::cout << (*pv).size() << std::endl;
    auto pv2 = std::move(pv);
    std::cout << &(*pv) << std::endl;
    std::cout << &(*pv2) << std::endl;
*/

} // mutty

#endif