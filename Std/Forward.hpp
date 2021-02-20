#pragma once

using usize = decltype(sizeof(int));

using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;
using i8 = signed char;
using i16 = signed short;
using i32 = signed int;
using i64 = signed long long;

static_assert(sizeof(u8) == 1);
static_assert(sizeof(u16) == 2);
static_assert(sizeof(u32) == 4);
static_assert(sizeof(u64) == 8);
static_assert(sizeof(i8) == 1);
static_assert(sizeof(i16) == 2);
static_assert(sizeof(i32) == 4);
static_assert(sizeof(i64) == 8);

constexpr void* operator new(usize count, void* ptr)
{
    return ptr;
}

constexpr void* operator new[](usize count, void* ptr)
{
    return ptr;
}

template<typename T>
constexpr T&& move(T &&value)
{
    return static_cast<T&&>(value);
}

template<typename T>
constexpr T&& forward(T &&value)
{
    return static_cast<T&&>(value);
}

template<typename T, typename U = T>
constexpr T exchange(T& obj, U&& new_value)
{
    T old_value = move(obj);
    obj = forward<U>(new_value);
    return old_value;
}
