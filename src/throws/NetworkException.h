#ifndef __NETWORK_EXCEPTION_H__
#define __NETWORK_EXCEPTION_H__
#include "MuttyException.h"
class NetworkException: public MuttyException {
    using MuttyException::MuttyException;
};
#endif