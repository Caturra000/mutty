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

/*

程序入口应该是looper还是server?
感觉上应该是server(looper)
auto context = server.accpet();

至于client就是一个比较特立独行的类了，因为跟looper无关

设计上，我的业务逻辑是放到handler中去的
外部类只需发个字段就能响应整个流程

可以说是个非常独立的类，独立到无需区分Server Client
并且Message中存的Pointer<Handler>是允许多态的

所以搞个Handler子类是很正常的

Handler中负责调用，暴露setter方法给外部
外部负责实现setOnXX(func)或者setOnXX([](ctx))
外部调用者就是Server/Client
因此结构上有了Handler/Message->Server/Client的依赖
Handler触发是根据Message来的
Server想要实现一个逻辑，需要：
1. Handler子类有一个逻辑，看到what就调用哪些observer
2. Server提供observer事件给Handler
3. 当TCP发生任何变化时，TCP层会发一个MSG到MQ，感觉这里是Context该做的事情，或者是另外一个新的类
4. Context也是可以改的，因为setOnXX([](ctx))是我们定义的


这里的微妙之处是
Handler并不知道网络层什么的，它只知道有Message就handle，怎么handle看你
Server/Client只需提供event给Handler，它不知道Message是啥
TCP只管状态变化就法MSG到MQ，它不知道Handler、Server/Client是啥

目前的矛盾在于Timer也有处理消息的一套
====>关联到Handler也是没问题的吧，只需提供一个TIME的MSG类型就好了，===好像还不行，需要转移Event=====需要提供更多的接口，比如对于time，每次处理完就删除event
====>似乎这个可以做成另一个TimerHandler！！！！有点搞头！ Timer方持有提供TimerHandler和Looper同款MQ，有事情post到那里去就好了（addEvent（copy的）, 然后post）

可以稍作修改应用于非网络的场合，如果只用TCP，然后调用方想知道的尽量的少
只需库提供一个TcpHandler，把默认的都TCP状态、MSG格式、eventSetter都搞好

到最后只需Server对着TCP的MSG格式写逻辑！完美

IMPROVE: Context改个名字吧，TcpContext也好

那么问题来了，
Server需要写逻辑给TcpHandler注册，与架构无关
looper的
*/

#endif