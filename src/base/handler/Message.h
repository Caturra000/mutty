#ifndef __MUTTY_MESSAGE_H__
#define __MUTTY_MESSAGE_H__
#include "utils/Pointer.h"
namespace mutty {

class Handler;

struct Message {
    Handler *target;
    int what;
    union {
        int uFlag;
        int uSize;
    };
    void *any;
};

} // mutty
#endif