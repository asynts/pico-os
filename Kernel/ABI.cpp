/*
* Copyright (c) 2021 Paul Scharnofske
* SPDX-License-Identifier: MIT
*/

#include <Std/Forward.hpp>

#include <Kernel/ABI.hpp>

#include <pico/bootrom.h>

extern "C"
usize strlen(const char *str)
{
    usize length = 0;
    while (*str++)
        length += 1;

    return length;
}

extern "C"
int strcmp(const char *a, const char *b)
{
    while (*a && *b) {
        if (*a < *b)
            return -1;

        if (*a > *b)
            return 1;

        ++a;
        ++b;
    }

    if (*a)
        return 1;

    if (*b)
        return -1;

    return 0;
}

extern "C"
int memcmp(const void *a_, const void *b_, usize n)
{
    const u8 *a = (const u8*)a_;
    const u8 *b = (const u8*)b_;

    for (usize i = 0; i < n; ++i) {
        if (*a < *b)
            return -1;

        if (*a > *b)
            return 1;

        ++a;
        ++b;
    }

    return 0;
}

extern "C"
int __aeabi_idiv0(int return_value)
{
    VERIFY_NOT_REACHED();
}

extern "C"
long long __aeabi_ldiv0(long long return_value)
{
    VERIFY_NOT_REACHED();
}

extern "C"
void __cxa_pure_virtual()
{
    VERIFY_NOT_REACHED();
}

extern "C"
void __cxa_bad_cast()
{
    VERIFY_NOT_REACHED();
}
