#ifndef __WEAK_SOCKET_H__
#define __WEAK_SOCKET_H__
#include "Socket.h"
// 一个不维护生命周期的socket wrapper，依然满足sizeof(fd)的大小
class WeakSocket: public Socket {
    ~WeakSocket() {} //FIXME:Socket和WeakSocket的关系应该倒置，这里析构不对
};
#endif