#ifndef __MUTTY_HANDLER_H__
#define __MUTTY_HANDLER_H__
#include <bits/stdc++.h>
#include "utils/Callable.h"
#include "utils/TypeTraits.h"
#include "utils/NonCopyable.h"
#include "Message.h"
namespace mutty {

#define HANDLER_CALLBACK_DEFINE(functionName, callbackMember, ContextType, contextPointer) \
    /* non-context */ \
    template <typename ...Args, typename = IsCallableType<Args...>> \
    void functionName(Args &&...args) { \
        callbackMember = Callable::make(std::forward<Args>(args)...); \
    } \
    /* simple context */ \
    template <typename Lambda, typename = IsCallableType<Lambda, ContextType*>> \
    void functionName(Lambda &&callback) { \
        callbackMember = Callable::make( \
            std::forward<Lambda>(callback), contextPointer); \
    } \
    /* safe for lifecycle */ \
    template <typename Lambda, typename U = IsCallableType<Lambda, std::weak_ptr<ContextType>>> \
    void functionName(Lambda &&callback, U* = nullptr) { \
        callbackMember = Callable::make( \
            std::forward<Lambda>(callback), std::weak_ptr<ContextType>(shared_from_this())); \
    }

class Handler: private NonCopyable {
public:
    virtual void handle(Message msg) = 0;
    virtual ~Handler() {}
};

} // mutty
#endif