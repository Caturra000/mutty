#ifndef __SOCKET_ERRNO_EXCEPTION_H__
#define __SOCKET_ERRNO_EXCEPTION_H__
#include "MuttyException.h"
class ErrnoException: public MuttyException {
private:
    int _err {0}; // 暂存errno
public:
    static constexpr const char *TAG = "errno exception";
    using MuttyException::MuttyException;
    ErrnoException(const char *info, int err)
        : MuttyException(info), _err(err) { }
    ErrnoException(int err): ErrnoException(TAG, err) { }
    int errorCode() { return _err; }
};
#endif