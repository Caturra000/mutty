#ifndef __HANDLER_H__
#define __HANDLER_H__
#include <bits/stdc++.h>

#include "utils/LazyEvaluate.h"
#include "utils/TypeTraits.h"
#include "Message.h"

#define HANDLER_CALLBACK_DEFINE(functionName, callbackMember, CtxFunctorType, bindedCtx) \
    template <typename ...Args, typename = IsCallableType<Args...>> \
    void functionName(Args &&...args) { \
        callbackMember = LazyEvaluate::lazy(std::forward<Args>(args)...); \
    } \
    void functionName(CtxFunctorType &&ctxFunctor) { \
        callbackMember = LazyEvaluate::lazy(std::forward<CtxFunctorType>(ctxFunctor), bindedCtx); \
    } 

class Handler {
public:
    // 处理消息的业务逻辑接口
    virtual void handle(Message msg) = 0;
    virtual ~Handler() { }
};

#endif