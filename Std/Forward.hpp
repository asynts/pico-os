#pragma once

using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;
using i8 = signed char;
using i16 = signed short;
using i32 = signed int;
using i64 = signed long long;

#ifdef TEST
using usize = long unsigned int;
using isize = i64;
#else
using usize = u32;
using isize = i32;
#endif

static_assert(sizeof(u8) == 1);
static_assert(sizeof(u16) == 2);
static_assert(sizeof(u32) == 4);
static_assert(sizeof(u64) == 8);
static_assert(sizeof(i8) == 1);
static_assert(sizeof(i16) == 2);
static_assert(sizeof(i32) == 4);
static_assert(sizeof(i64) == 8);

#ifndef TEST
constexpr void* operator new(usize count, void* ptr)
{
    return ptr;
}
constexpr void* operator new[](usize count, void* ptr)
{
    return ptr;
}
// Defined by the SDK.
void* operator new(usize);
void* operator new[](usize);
void operator delete(void*);
void operator delete[](void*);
#endif

template<typename T>
struct RemoveReference {
    using Type = T;
};
template<typename T>
struct RemoveReference<T&> {
    using Type = T;
};

template<typename T>
constexpr typename RemoveReference<T>::Type&& move(T&& value)
{
    return static_cast<typename RemoveReference<T>::Type&&>(value);
}

template<typename T>
constexpr T&& forward(typename RemoveReference<T>::Type& value)
{
    return static_cast<T&&>(value);
}
template<typename T>
constexpr T&& forward(typename RemoveReference<T>::Type&& value)
{
    return static_cast<T&&>(value);
}

template<typename T, typename U = T>
constexpr T exchange(T& obj, U&& new_value)
{
    T old_value = move(obj);
    obj = forward<U>(new_value);
    return move(old_value);
}

template<typename T>
constexpr void swap(T& lhs, T& rhs)
{
    T value = move(lhs);
    lhs = move(rhs);
    rhs = move(value);
}

inline void strlcpy(char *destination, const char *source, usize size)
{
    if (size >= 1) {
        __builtin_strncpy(destination, source, size - 1);
        destination[size - 1] = 0;
    }
}

inline void* memcpy(void *destination, const void *source, usize count)
{
    return __builtin_memcpy(destination, source, count);
}

template<typename T>
constexpr T max(T a, T b)
{
    return a >= b ? a : b;
}

template<typename T>
constexpr T min(T a, T b)
{
    return a <= b ? a : b;
}

inline usize round_to_power_of_two(usize value)
{
    return 1 << (32 - __builtin_clz(value));
}

namespace Std {
    enum class IterationDecision {
        Continue,
        Break,
    };

    class Path;

    template<typename... Parameters>
    void dbgln(const char *fmtstr, const Parameters&...);
}

[[noreturn]]
void __crash(const char *format, ...);

#define ASSERT(condition) ((condition) ? (void)0 : __crash("ASSERT(%s)\n%s:%i\n", #condition, __FILE__, __LINE__))
#define VERIFY(condition) ((condition) ? (void)0 : __crash("VERIFY(%s)\n%s:%i\n", #condition, __FILE__, __LINE__))

#define ASSERT_NOT_REACHED() __crash("ASSERT_NOT_REACHED():\n%s:%i\n", __FILE__, __LINE__)
#define VERIFY_NOT_REACHED() __crash("VERIFY_NOT_REACHED():\n%s:%i\n", __FILE__, __LINE__)

#define FIXME() __crash("FIXME()\n%s:%i\n", __FILE__, __LINE__)
#define FIXME_ASSERT(condition) ((condition) ? (void)0 : __crash("FIXME(%s)\n%s:%i\n", #condition, __FILE__, __LINE__))
