#pragma once

using u8    = unsigned char;
using u16   = unsigned short;
using u32   = unsigned int;
using u64   = unsigned long long;
using uptr  = unsigned long;
using usize = __SIZE_TYPE__;

using i8    = signed char;
using i16   = signed short;
using i32   = signed int;
using i64   = signed long long;
using iptr  = signed long;

static_assert(sizeof(uptr) == sizeof(int*));
static_assert(sizeof(uptr) == sizeof(void(*)()));
static_assert(sizeof(decltype(sizeof(int))) == sizeof(usize));

inline void* operator new(usize, void *pointer) {
    return pointer;
}
inline void* operator new[](usize, void *pointer) {
    return pointer;
}

constexpr usize KiB = 1024;
constexpr usize MiB = 1024 * 1024;
constexpr usize GiB = 1024 * 1024 * 1024;

extern "C"
void* memcpy(void *destination, const void *source, usize count);
