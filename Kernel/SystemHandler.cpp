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
    extern "C"
    i32 syscall_handler(FullRegisterContext *context)
    {
        auto& process = Process::active_process();
        auto& thread = Scheduler::the().active_thread();

        VERIFY(!thread.m_context.is_valid());
        thread.m_context = context;

        i32 return_value;
        if (context->r0.syscall() == _SC_read)
            return_value = process.sys$read(context->r1.fd(), context->r2.pointer<u8>(), context->r3.value<usize>());
        else if (context->r0.syscall() == _SC_write)
            return_value = process.sys$write(context->r1.fd(), context->r2.pointer<const u8>(), context->r3.value<usize>());
        else if (context->r0.syscall() == _SC_open)
            return_value = process.sys$open(context->r1.cstring(), context->r2.value<u32>(), context->r3.value<u32>());
        else if (context->r0.syscall() == _SC_close)
            return_value = process.sys$close(context->r1.fd());
        else if (context->r0.syscall() == _SC_fstat)
            return_value = process.sys$fstat(context->r1.fd(), context->r2.pointer<UserlandFileInfo>());
        else if (context->r0.syscall() == _SC_fork)
            return_value = process.sys$fork();
        else if (context->r0.syscall() == _SC_wait)
            return_value = process.sys$wait(context->r1.pointer<i32>());
        else if (context->r0.syscall() == _SC_execve)
            return_value = process.sys$execve(context->r1.cstring(), context->r2.pointer<char*>(), context->r3.pointer<char*>());
        else
            VERIFY_NOT_REACHED();

        VERIFY(&thread == &Scheduler::the().active_thread());
        VERIFY(context == thread.m_context.must());
        thread.m_context.clear();

        return return_value;
    }
}
