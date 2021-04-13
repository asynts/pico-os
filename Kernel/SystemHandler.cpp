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
        else if (context->r0.syscall() == _SC_fstat)
            return process.sys$fstat(context->r1.fd(), context->r2.pointer<UserlandFileInfo>());
        else if (context->r0.syscall() == _SC_fork)
            return process.sys$fork();

        VERIFY_NOT_REACHED();
    }
}
