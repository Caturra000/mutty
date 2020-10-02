#ifndef __MESSAGE_H__
#define __MESSAGE_H__
#include "utils/Pointer.h"

class Handler;

// 表明一条TCP连接在发生某些事件时的消息
// 消息应该是轻量级的，可用值语义传递，且不持有任何生命周期
struct Message {
    Pointer<Handler> _target; // 归属的handler，不存在可能delete的风险
    int _what; // 表明一个什么样的事件，给handler看时可以通过switch case来确定具体的回调   
    int _flag;  // 由于_what和target有内存对齐的空洞，所以多设计一个arg，补充对_what的说明
    // std::any object; // 用于传递一些大的对象（的引用？）
    // 至于what和flag具体是什么，不同handler有不同定义
};

#endif