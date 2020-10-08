#ifndef __HANDLER_H__
#define __HANDLER_H__
#include <bits/stdc++.h>

#include "utils/LazyEvaluate.h"
#include "Message.h"


// 由于template接口太丑的原因多加了一层macro
// 规范：MSG_<Handler前缀>_<细分消息类型>
// #define HANDLER_MSG_DEFINE(name) \
//     const static int name = __LINE__
#define HANDLER_CALLBACK_DEFINE(functionName, callbackMember) \
    template <typename Functor, typename ...Args> \
    void functionName(Functor &&functor, Args &&...args) { \
        callbackMember = LazyEvaluate::lazy(std::forward<Functor>(functor), std::forward<Args>(args)...); \
    }


class Handler {
public:
    // 处理消息的业务逻辑接口
    virtual void handle(Message msg) = 0;
    virtual ~Handler() { }
};

#endif