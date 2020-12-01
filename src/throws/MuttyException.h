#ifndef __MUTTY_EXCEPTION_H__
#define __MUTTY_EXCEPTION_H__
#include <bits/stdc++.h>
class MuttyException: public std::exception {
private:
    std::string _info;
public:
    explicit MuttyException(std::string info)
        : _info(std::move(info)) { }
    const char* what() const noexcept override {
        return _info.c_str();
    }
};
#endif