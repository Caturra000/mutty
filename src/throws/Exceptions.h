#ifndef __THROWS_EXCEPTIONS_H__
#define __THROWS_EXCEPTIONS_H__

#include "MuttyException.h"
    #include "ErrnoException.h"
        #include "NetworkException.h"
            #include "SocketException.h"
                #include "SocketCreateException.h"
                #include "SocketBindException.h"
                #include "SocketListenException.h"
                #include "SocketAcceptException.h"
        #include "IoException.h"
            #include "ReadException.h"
            #include "WriteException.h"
        #include "EpollException.h"
            #include "EpollCreateException.h"
            #include "EpollWaitException.h"

#endif