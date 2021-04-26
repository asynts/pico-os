#pragma once

extern "C"
usize strlen(const char *str);

extern "C"
int strcmp(const char *a, const char *b);

extern "C"
int memcmp(const void *a, const void *b, usize n);

extern "C"
int __aeabi_idiv0(int return_value);

extern "C"
long long __aeabi_ldiv0(long long return_value);

extern "C"
void __cxa_pure_virtual();

extern "C"
void __cxa_bad_cast();
