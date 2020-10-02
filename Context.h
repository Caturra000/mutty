#ifndef __CONTEXT_H__
#define __CONTEXT_H__
#include "utils/Pointer.h"
#include "Looper.h"
#include "Server.h"
#include "Client.h"
// 表示一条连接，及其所对应的回调上下文
class Context {
public:
    void xjbTest() {
        
    }
    // registerXX
    Context(Pointer<Looper> looper): _looper(looper) {}
private:
    Pointer<Looper> _looper;
    Pointer<Server> _server;
    Pointer<Client> _client;
};



#endif