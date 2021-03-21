#include <Std/Forward.hpp>
#include <Kernel/Interface/syscalls.h>
#include <Kernel/ConsoleDevice.hpp>
#include <Kernel/FileSystem/MemoryFileSystem.hpp>
#include <Kernel/File.hpp>

#define STDIN_FILENO 0
#define STDOUT_FILENO 1

class TypeErasedArgument {
public:
    i32 fd() const { return *reinterpret_cast<const i32*>(&m_value); }
    usize size() const { return m_value; }

    template<typename T>
    T* pointer() { return reinterpret_cast<T*>(m_value); }

    template<typename T>
    T value() { return static_cast<T>(m_value); }

private:
    u32 m_value;
};

extern "C"
isize isr_svcall(u32 syscall, TypeErasedArgument arg1, TypeErasedArgument arg2, TypeErasedArgument arg3)
{
    if (syscall == _SC_read) {
        i32 fd = arg1.fd();
        char *buffer = arg2.pointer<char>();
        usize count = arg3.size();

        assert(fd == STDIN_FILENO);

        Kernel::ConsoleDevice::the().read({ (u8*)buffer, count });
        return count;
    }

    if (syscall == _SC_write) {
        i32 fd = arg1.fd();
        const char *buffer = arg2.pointer<const char>();
        usize count = arg3.size();

        assert(fd == STDOUT_FILENO);

        Kernel::ConsoleDevice::the().write({ (u8*)buffer, count });
        return count;
    }

    if (syscall == _SC_open) {
        const char *path = arg1.pointer<const char>();
        u32 flags = arg2.value<u32>();
        u32 mode = arg3.value<u32>();

        auto absolute_path = Kernel::compute_absolute_path(path);

        auto& entry = Kernel::MemoryFileSystem::the().lookup_path(absolute_path.view());

        if (flags & O_DIRECTORY) {
            if (entry.m_info->m_mode & S_IFMT != S_IFDIR)
                return -ENOTDIR;
        }

        auto& file = *new Kernel::File { entry };
        auto& handle = *new Kernel::FileHandle { file, Kernel::FileHandle::generate_fd() };

        return handle.fd();
    }

    panic("Unknown system call %i", syscall);
}
