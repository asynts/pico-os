#pragma once

using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;
using i8 = signed char;
using i16 = signed short;
using i32 = signed int;
using i64 = signed long long;

#if defined(TEST) || defined(HOST)
using usize = long unsigned int;
using isize = i64;
using uptr = u64;
#else
using usize = u32;
using isize = i32;
using uptr = u32;
#endif

static_assert(sizeof(u8) == 1);
static_assert(sizeof(u16) == 2);
static_assert(sizeof(u32) == 4);
static_assert(sizeof(u64) == 8);
static_assert(sizeof(i8) == 1);
static_assert(sizeof(i16) == 2);
static_assert(sizeof(i32) == 4);
static_assert(sizeof(i64) == 8);
static_assert(sizeof(usize) == sizeof(decltype(sizeof(int))));
static_assert(sizeof(isize) == sizeof(decltype(sizeof(int))));
static_assert(sizeof(uptr) == sizeof(void*));
