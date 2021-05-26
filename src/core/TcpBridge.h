#ifndef __MUTTY_TCP_BRIDGE_H__
#define __MUTTY_TCP_BRIDGE_H__
#include "TcpContext.h"
#include "TcpHandler.h"
namespace mutty {

inline void TcpHandler::handleRead() {
    ssize_t n = _context->inputBuffer.readFrom(_context->acceptedSocket.fd());
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
        ssize_t n = _context->outputBuffer.writeTo(_context->acceptedSocket.fd());
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
        // try to shutdown if force close
        _context->shutdown();
        // shouble be DISCONNECTING
        _context->disableRead();
        _context->disableWrite();
        auto context = _context.get();
        MUTTY_LOG_DEBUG("send async disconnected message. hash =", _context->hashcode());
        // the last message of context
        _context->async([this, context] {
            context->setDisConnected();
            _closeCallback();
        });
    }
}

inline void TcpHandler::handleStart() {
    if(_context->isConnecting()) {
        _context->setConnected();
        _context->enableRead();
        _connectionCallback();
        MUTTY_LOG_INFO("connection started. info:", _context->simpleInfo());
        MUTTY_LOG_DEBUG("[+] connection started. hash =", _context->hashcode());
    }
}

} // mutty
#endif