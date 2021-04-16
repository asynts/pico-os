#pragma once

#include <Std/Forward.hpp>

namespace Kernel
{
    struct TypeErasedValue {
        template<typename T>
        T value() { return bit_cast<T>(m_storage); }

        template<typename T>
        T* pointer() { return bit_cast<T*>(m_storage); }

        u32 syscall() { return value<u32>(); }
        const char* cstring() { return pointer<const char>(); }
        i32 fd() { return value<i32>(); }

        u32 m_storage;
    };

    struct ExceptionRegisterContext {
        TypeErasedValue r0;
        TypeErasedValue r1;
        TypeErasedValue r2;
        TypeErasedValue r3;
        TypeErasedValue ip;
        TypeErasedValue lr;
        TypeErasedValue pc;
        TypeErasedValue xpsr;
    };

    struct FullRegisterContext {
        TypeErasedValue r11;
        TypeErasedValue r10;
        TypeErasedValue r9;
        TypeErasedValue r8;
        TypeErasedValue r7;
        TypeErasedValue r6;
        TypeErasedValue r5;
        TypeErasedValue r4;

        TypeErasedValue r0;
        TypeErasedValue r1;
        TypeErasedValue r2;
        TypeErasedValue r3;
        TypeErasedValue ip;
        TypeErasedValue lr;
        TypeErasedValue pc;
        TypeErasedValue xpsr;
    };
}
