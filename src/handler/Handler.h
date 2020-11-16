#ifndef __HANDLER_H__
#define __HANDLER_H__
#include <bits/stdc++.h>

#include "utils/LazyEvaluate.h"
#include "utils/TypeTraits.h"
#include "Message.h"

#define HANDLER_CALLBACK_DEFINE(functionName, callbackMember, ContextType, ContextPtr) \
    template <typename ...Args, typename = IsCallableType<Args...>> \
    void functionName(Args &&...args) { \
        callbackMember = LazyEvaluate::lazy(std::forward<Args>(args)...); \
    } \
    void functionName(std::function<void(ContextType*)> &&ctxFunctor) { \
        callbackMember = LazyEvaluate::lazy( \
            std::forward<std::function<void(ContextType*)>>(ctxFunctor), ContextPtr); \
    }

#define HANDLER_WEAK_CALLBACK_DEFINE(functionName, callbackMember, ContextType, sharedFrom) \
    void functionName(std::function<void(std::weak_ptr<ContextType>)> &&ctxFunctor) { \
        callbackMember = LazyEvaluate::lazy( \
            std::forward<std::function<void(std::weak_ptr<ContextType>)>>(ctxFunctor), sharedFrom); \
    }

class Handler {
public:
    // 处理消息的业务逻辑接口
    virtual void handle(Message msg) = 0;
    virtual ~Handler() { }
};

#endif