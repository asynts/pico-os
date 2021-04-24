#pragma once

extern int errno;

char* strerror(int error);

#define libc_check_errno(variable) \
    do { \
        if (variable < 0) { \
            errno = -variable; \
            return -1; \
        } \
    } while(0)
