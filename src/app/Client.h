#ifndef __CLIENT_H__
#define __CLIENT_H__
#include <bits/stdc++.h>
#include "utils/Pointer.h"
#include "core/Looper.h"
#include "net/InetAddress.h"
class Client {
public:
    Client(Looper *looper, InetAddress serverAddress)
        : _looper(looper) {}
private:
    Pointer<Looper> _looper;
};
#endif