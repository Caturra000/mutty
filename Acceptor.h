#ifndef __ACCEPTOR_H__
#define __ACCEPTOR_H__
#include <bits/stdc++.h>
#include "Socket.h"
#include "InetAddress.h"
#include "Handler.h"
#include "AcceptHandler.h"
#include "MessageQueue.h"
class Acceptor {
public:
    Acceptor(const InetAddress &localAddress):  _localAddress(localAddress) {
        using Option = Socket::Option;
        _acceptSocket.config(Option::REUSE_PORT | Option::REUSE_ADDR);
        _acceptSocket.bind(localAddress);
        // _acceptSocket.listen();
        // do accept in Looper =====> post一条消息到MQ里头
    }

    // TODO 是否应返回Socket&/对socket进行public

    int fd() const { return _acceptSocket.fd(); } // 提供给Multiplexer
    InetAddress localAddress() const { return _localAddress; } // copy，不希望考虑生命周期

private:
    Socket _acceptSocket; 
    InetAddress _localAddress;

    std::unique_ptr<AcceptHandler> _handlers;
    Pointer<MessageQueue> _messageQueue;

};
#endif