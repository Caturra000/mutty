#ifndef __MUTTY_ACCEPT_CONTEXT_H__
#define __MUTTY_ACCEPT_CONTEXT_H__
#include <fcntl.h>
#include "utils/Pointer.h"
#include "utils/Exchanger.h"
#include "net/Socket.h"
#include "net/InetAddress.h"
#include "ContextImpl.h"
namespace mutty {

class Handler;
class AcceptContext: public ContextImpl {
public:

    constexpr static int MSG_SOCKET_LISTEN     = 10;
    constexpr static int MSG_ACCEPT_WITH_DATA  = 11;

    int fd() const override { return acceptSocket.fd(); }

    Socket acceptSocket;
    InetAddress localAddress;

    Exchanger exchanger;

    AcceptContext(Handler *handler, Looper *looper, InetAddress localAddress);
};

inline AcceptContext::AcceptContext(Handler *handler, Looper *looper, InetAddress localAddress)
    : ContextImpl(handler, looper),
      localAddress(localAddress) {
    using Option = Socket::Option;
    acceptSocket.config(Option::REUSE_PORT | Option::REUSE_ADDR);
    acceptSocket.bind(localAddress);
}

} // mutty
#endif