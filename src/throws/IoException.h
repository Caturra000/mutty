#ifndef __MUTTY_IO_EXCEPTION_H__
#define __MUTTY_IO_EXCEPTION_H__
#include "ErrnoException.h"
namespace mutty {

class IoException: public ErrnoException {
    using ErrnoException::ErrnoException;
};

} // mutty
#endif