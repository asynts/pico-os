#include <Std/Forward.hpp>
#include <Kernel/Interface/syscalls.h>
#include <Kernel/ConsoleDevice.hpp>
#include <Kernel/FileSystem/MemoryFileSystem.hpp>
#include <Kernel/Process.hpp>
#include <Kernel/FileSystem/FileSystem.hpp>

#include <Kernel/Interface/stat.h>

#define STDIN_FILENO 0
#define STDOUT_FILENO 1

namespace Kernel
{
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
    static_assert(sizeof(TypeErasedArgument) == sizeof(u32));
    static_assert(alignof(TypeErasedArgument) == alignof(u32));

    extern "C"
    isize syscall_handler(u32 syscall, TypeErasedArgument arg1, TypeErasedArgument arg2, TypeErasedArgument arg3)
    {
        if (syscall == _SC_read) {
            i32 fd = arg1.fd();
            auto *buffer = arg2.pointer<u8>();
            usize count = arg3.size();

            if (fd > 2)
                dbgln("[syscall_handler] read({}, {}, {})", fd, buffer, count);

            auto& handle = Kernel::Process::current().get_file_handle(fd);
            return handle.read({ buffer, count }).must();
        }

        if (syscall == _SC_write) {
            i32 fd = arg1.fd();
            auto *buffer = arg2.pointer<const u8>();
            usize count = arg3.size();

            if (fd > 2)
                dbgln("[syscall_handler] write({}, {}, {})", fd, buffer, count);

            auto& handle = Kernel::Process::current().get_file_handle(fd);
            return handle.write({ buffer, count }).must();
        }

        if (syscall == _SC_open) {
            Path path = arg1.pointer<const char>();
            u32 flags = arg2.value<u32>();
            u32 mode = arg3.value<u32>();

            dbgln("[syscall_handler] open({}, {}, {})", path, flags, mode);

            if (!path.is_absolute())
                path = Process::current().m_working_directory / path;

            auto& file = Kernel::FileSystem::lookup(path);

            if ((flags & O_DIRECTORY)) {
                if ((file.m_mode & ModeFlags::Format) != ModeFlags::Directory) {
                    dbgln("[syscall_handler] Not a directory, ENOTDIR");
                    return -ENOTDIR;
                }
            }

            auto& handle = file.create_handle();
            return Kernel::Process::current().add_file_handle(handle);
        }

        if (syscall == _SC_close) {
            dbgln("[syscall_handler] _SC_close not implemented");
            return 0;
        }

        if (syscall == _SC_fstat) {
            i32 fd = arg1.fd();
            UserlandFileInfo *statbuf = arg2.pointer<UserlandFileInfo>();

            auto& handle = Kernel::Process::current().get_file_handle(fd);
            auto& file = handle.file();

            // FIXME: We need some sensible values here
            statbuf->st_dev = 0xdead;
            statbuf->st_rdev = 0xdead;
            statbuf->st_size = 0xdead;
            statbuf->st_blksize = 0xdead;
            statbuf->st_blocks = 0xdead;

            statbuf->st_ino = file.m_ino;
            statbuf->st_mode = static_cast<u32>(file.m_mode);
            statbuf->st_uid = file.m_owning_user;
            statbuf->st_gid = file.m_owning_group;

            return 0;
        }

        if (syscall == _SC_stat) {
            Path path = arg1.pointer<const char>();
            UserlandFileInfo *statbuf = arg2.pointer<UserlandFileInfo>();

            if (!path.is_absolute())
                path = Process::current().m_working_directory / path;

            auto& file = FileSystem::lookup(path);

            // FIXME: We need some sensible values here
            statbuf->st_dev = 0xdead;
            statbuf->st_rdev = 0xdead;
            statbuf->st_size = 0xdead;
            statbuf->st_blksize = 0xdead;
            statbuf->st_blocks = 0xdead;

            statbuf->st_ino = file.m_ino;
            statbuf->st_mode = static_cast<u32>(file.m_mode);
            statbuf->st_uid = file.m_owning_user;
            statbuf->st_gid = file.m_owning_group;

            return 0;
        }

        VERIFY_NOT_REACHED();
    }
}
