#ifndef __SERVER_H__
#define __SERVER_H__
#include <bits/stdc++.h>
#include "utils/Pointer.h"
#include "Looper.h"
#include "MessageQueue.h"
// 使用socket内部类的更明确的封装
class Server {
    // listenFd
    Pointer<Looper> _looper; // under _looper controll
    Pointer<MessageQueue> _messageQueue; // looper和mq的生命周期长于Server，且mq来自于_looper （为了方便还是拿出来了）
    //
};
#endif