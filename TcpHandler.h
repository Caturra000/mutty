#ifndef __TCP_HANDLER_H__
#define __TCP_HANDLER_H__
#include <bits/stdc++.h>
#include "Handler.h"
#include "Message.h"
#include "TcpFeature.h"
class TcpHandler: public Handler, public TcpFeature {
public:

    HANDLER_MSG_DEFINE(MSG_TCP_ON_ACCEPT);

    virtual void handle(Message msg) override {
        switch(msg._what) {
            case MSG_TCP_ON_ACCEPT:
            break;
            default:
            break;
        }
    }
};
#endif