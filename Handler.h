#ifndef __HANDLER_H__
#define __HANDLER_H__
#include <bits/stdc++.h>

#include "utils/LazyEvaluate.h"
#include "Message.h"
// 期望是这样
// register(func, args...) // 这个func就是对应的onXX方法
// 

// 问题一：是否应该用vector来对同一event进行listen（muduo是缺少这种特性支持的）
// 问题二：是否应该在同一Handler下处理多种event（muduo是这么写的）
// 问题三：如果否认问题二，那么应该是继承多个类还是用组合直接改？----->从函数签名的角度还是前者较好，该类的行为并不需要很灵活的处理
class Handler {
private:
    std::vector<LazyEvaluate> _connectedObservers;
    std::vector<LazyEvaluate> _readObservers;
    std::vector<LazyEvaluate> _readFinishedObservers;
    std::vector<LazyEvaluate> _closeObservers;
    // std::unique_ptr<Context> _context // Context&
public:
    void registerOnConnected(LazyEvaluate callback) {
        _connectedObservers.emplace_back(std::move(callback));
    }

    // IMPROVEMENT: 提供一个ContextCallback
    // 或者另开一个xxContextListener; 这样对于一个用到ctx的回调，只需声明[](ctx){}函数，
    // 在另开的签名中/或者编译时检测 存入对应listeners，调用过程分两次调用即可，一次是listener.evalute() 另一次是ctxListener.evalute(传入ctx)
    // 这样要求LE更改内部实现，有点难啊
    // 或者干脆就给一个std::function<void(ctx)>的listener补一下就好吧，而不用支持更为宽泛的<void(Args...)>这种东西，毕竟对Args单独存储也太累了，相当于重新实现bind（草
    // 或者我提供一个代理，对于[](Args...){}我都用Proxy魔改为[](){}，然后另一个客户对proxy回调一个带参的std::bind(ServerArgs*, ...)就可以了 ----> 那么问题来了，我怎么知道(Args)

    // 接受一个lambda，固定参数为[](ctx) {...}
    void registerOnConnected(/*Context &ctx*/std::function<void(void*)> callMeLater) {

    }

    // IMPROVMENT: 添加弱回调注册

    void onConnected() {
        for(auto &&observer : _connectedObservers) {
            observer.evaluate();
        }
    }

    // 处理消息的业务逻辑接口
    void handle(Message msg) {
        switch(msg._what) {
            default:;
        }
    }
};

#endif