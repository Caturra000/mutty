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

    int fd() const override { return acceptSocket.fd(); }
    uint32_t events() const override { return 0; }

    // Socket connectSocket;
    // InetAddress peerAddress;
    // int acceptedId;

    Socket acceptSocket;
    InetAddress localAddress;


    // send {flag = id} TODO

    // AcceptContext(Handler *handler, Looper *looper, Socket connectSocket, 
    //     const InetAddress &localAddress, const InetAddress &peerAddress)
    //     : ContextImpl(handler, looper),
    //       connectSocket(std::move(connectSocket)),
    //       localAddress(localAddress),
    //       peerAddress(peerAddress) { }
          
};
#endif