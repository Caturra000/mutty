#ifndef __HANDLER_H__
#define __HANDLER_H__
#include <bits/stdc++.h>

#include "utils/LazyEvaluate.h"
#include "Message.h"


// 规范：MSG_<Handler前缀>_<细分消息类型>
#define HANDLER_MSG_DEFINE(name) \
    const static int name = __LINE__




class Handler {
private:
    // std::vector<LazyEvaluate> _connectedObservers;
    // std::vector<LazyEvaluate> _readObservers;
    // std::vector<LazyEvaluate> _readFinishedObservers;
    // std::vector<LazyEvaluate> _closeObservers;
    // std::unique_ptr<Context> _context // Context&
public:
    // void registerOnConnected(LazyEvaluate callback) {
    //     _connectedObservers.emplace_back(std::move(callback));
    // }

    



    // 处理消息的业务逻辑接口
    virtual void handle(Message msg) = 0;
    virtual ~Handler() { }
    
};

#endif