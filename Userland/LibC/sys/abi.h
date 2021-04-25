#pragma once

#include <sys/types.h>

void __aeabi_memcpy(void *dest, const void *src, size_t n);
void __aeabi_memset(void *dest, size_t n, int c);
void __aeabi_memclr(void *dest, size_t n);
void __aeabi_memmove(void *dest, const void *src, size_t n);
