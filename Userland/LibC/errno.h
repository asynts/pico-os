#pragma once

#include <sys/system.h>

extern int __libc_errno;
#define errno access_mutable_global(__libc_errno, int)

char* strerror(int error);

#define libc_check_errno(variable) \
    do { \
        if (variable < 0) { \
            errno = -variable; \
            return -1; \
        } \
    } while(0)
