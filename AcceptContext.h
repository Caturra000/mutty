#ifndef __ACCEPT_CONTEXT_H
#define __ACCEPT_CONTEXT_H
#include "utils/Pointer.h"
#include "ContextImpl.h"
#include "Socket.h"
#include "InetAddress.h"
class Handler;
class AcceptContext: public ContextImpl {
public:

    CONTEXT_MSG_DEFINE(MSG_SOCKET_LISTEN);
    CONTEXT_MSG_DEFINE(MSG_ACCEPT_WITH_DATA);

    int fd() const override { return acceptSocket.fd(); }
    uint32_t events() const override { return 0; }



    Socket acceptSocket;
    InetAddress localAddress;


    // send {flag = id} TODO

    AcceptContext(Handler *handler, Looper *looper, 
        Socket acceptSocket, InetAddress localAddress)
        : ContextImpl(handler, looper),
          acceptSocket(std::move(acceptSocket)),
          localAddress(localAddress) { } // TODO add timer...
};
#endif