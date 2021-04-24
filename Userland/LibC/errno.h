#pragma once

extern int errno;

#define libc_check_errno(variable) \
    do { \
        if (variable < 0) { \
            errno = -variable; \
            return -1; \
        } \
    } while(0)
