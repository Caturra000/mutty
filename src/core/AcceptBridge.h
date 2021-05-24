#ifndef __MUTTY_ACCEPT_BRIDGE_H__
#define __MUTTY_ACCEPT_BRIDGE_H__
#include "AcceptContext.h"
#include "AcceptHandler.h"
namespace mutty {

inline void AcceptHandler::handleRead(int who) {
    InetAddress peerAddress;
    Socket connectSocket = _context->acceptSocket.accept(peerAddress);
    _context->exchanger = std::pair<Socket, InetAddress>(
        std::move(connectSocket), std::move(peerAddress));
    _newConnectionCallback(); // add to ConnectionPool of Server
}

} // mutty
#endif