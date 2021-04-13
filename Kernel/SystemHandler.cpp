#include <Std/Forward.hpp>

#include <Kernel/Interface/System.hpp>
#include <Kernel/ConsoleDevice.hpp>
#include <Kernel/FileSystem/MemoryFileSystem.hpp>
#include <Kernel/Process.hpp>
#include <Kernel/FileSystem/FileSystem.hpp>
#include <Kernel/Interface/Types.hpp>
#include <Kernel/Scheduler.hpp>

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

    struct RegisterContext {
        // Restored by exception handler
        TypeErasedValue r11;
        TypeErasedValue r10;
        TypeErasedValue r9;
        TypeErasedValue r8;
        TypeErasedValue r7;
        TypeErasedValue r6;
        TypeErasedValue r5;
        TypeErasedValue r4;

        // Restored on exception return
        TypeErasedValue r0;
        TypeErasedValue r1;
        TypeErasedValue r2;
        TypeErasedValue r3;
        TypeErasedValue ip;
        TypeErasedValue lr;
        TypeErasedValue pc;
        TypeErasedValue xpsr;
    };

    extern "C"
    i32 syscall_handler(RegisterContext *context)
    {
        auto& process = Process::active_process();

        if (context->r0.syscall() == _SC_read)
            return process.sys$read(context->r1.fd(), context->r2.pointer<u8>(), context->r3.value<usize>());
        else if (context->r0.syscall() == _SC_write)
            return process.sys$write(context->r1.fd(), context->r2.pointer<const u8>(), context->r3.value<usize>());
        else if (context->r0.syscall() == _SC_open)
            return process.sys$open(context->r1.cstring(), context->r2.value<u32>(), context->r3.value<u32>());
        else if (context->r0.syscall() == _SC_close)
            return process.sys$close(context->r1.fd());

        VERIFY_NOT_REACHED();
    }
}
