#ifndef __ACCEPT_CONTEXT_H
#define __ACCEPT_CONTEXT_H
#include "utils/Pointer.h"
#include "DefaultContext.h"
#include "Socket.h"
#include "InetAddress.h"
class Handler;
class AcceptContext: public DefaultContext {
public:

    CONTEXT_MSG_DEFINE(MSG_SOCKET_LISTEN);

    int fd() const override { return connectSocket.fd(); }
    uint32_t events() const override { return 0; }

    Socket connectSocket;
    InetAddress localAddress;
    InetAddress peerAddress;
    int acceptedId;

    // send {flag = id} TODO

    AcceptContext(Handler *handler, MessageQueue *messageQueue, 
        Socket connectSocket, const InetAddress &localAddress, const InetAddress &peerAddress)
        : DefaultContext(handler, messageQueue),
          connectSocket(std::move(connectSocket)),
          localAddress(localAddress),
          peerAddress(peerAddress) { }
          
};
#endif