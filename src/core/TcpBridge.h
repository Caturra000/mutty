#ifndef __MUTTY_TCP_BRIDGE_H__
#define __MUTTY_TCP_BRIDGE_H__
#include "TcpContext.h"
#include "TcpHandler.h"
namespace mutty {

inline void TcpHandler::handleRead() {
    int n = _context->inputBuffer.readFrom(_context->acceptedSocket.fd());
    if(n > 0) {
        _messageCallback();
    } else if(n == 0) { // FIN
        handleClose();
    } else {
        handleError();
    }
}

inline void TcpHandler::handleWrite() {
    if(_context->writeEnabled()) {
        int n = _context->outputBuffer.writeTo(_context->acceptedSocket.fd()); // can async?
        if(n > 0) {
            if(_context->outputBuffer.unread() == 0) {
                _context->disableWrite();
                _writeCompleteCallback();
            }
        }
    }
}

inline void TcpHandler::handleError() {
    throw MuttyException("error callback");
}

inline void TcpHandler::handleClose() {
    if(_context->isConnected() || _context->isDisConnecting()) {
        _context->disableRead();
        _context->disableWrite();
        _context->setDisConnected();
        _closeCallback();
    }
}

} // mutty
#endif