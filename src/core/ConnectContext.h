#ifndef __CONNECT_CONTEXT_H__
#define __CONNECT_CONTEXT_H__
#include <bits/stdc++.h>
#include "base/context/ContextImpl.h"

class ConnectContext: public ContextImpl {
public:

    ConnectContext(Handler *handler, Looper *looper)
        : ContextImpl(handler, looper) {}
};
#endif