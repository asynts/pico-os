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
    isize syscall_handler(u32 syscall, TypeErasedArgument arg1, TypeErasedArgument arg2, TypeErasedArgument arg3, u32 return_address)
    {
        VERIFY(return_address >= 0x10000000 && return_address < 0x20000000);

        // FIXME: Ensure that the scheduler doesn't switch before we grab this
        auto& process = Process::active_process();

        if (syscall == _SC_read) {
            i32 fd = arg1.fd();
            auto *buffer = arg2.pointer<u8>();
            usize count = arg3.size();

            if (fd > 2)
                dbgln("[syscall_handler] read({}, {}, {})", fd, buffer, count);

            auto& handle = process.get_file_handle(fd);
            return handle.read({ buffer, count }).must();
        }

        if (syscall == _SC_write) {
            i32 fd = arg1.fd();
            auto *buffer = arg2.pointer<const u8>();
            usize count = arg3.size();

            if (fd > 2)
                dbgln("[syscall_handler] write({}, {}, {})", fd, buffer, count);

            auto& handle = process.get_file_handle(fd);
            return handle.write({ buffer, count }).must();
        }

        if (syscall == _SC_open) {
            Path path = arg1.pointer<const char>();
            u32 flags = arg2.value<u32>();
            u32 mode = arg3.value<u32>();

            dbgln("[syscall_handler] open({}, {}, {})", path, flags, mode);

            if (!path.is_absolute())
                path = process.m_working_directory / path;

            auto& file = Kernel::FileSystem::lookup(path);

            if ((flags & O_DIRECTORY)) {
                if ((file.m_mode & ModeFlags::Format) != ModeFlags::Directory) {
                    dbgln("[syscall_handler] Not a directory, ENOTDIR");
                    return -ENOTDIR;
                }
            }

            auto& handle = file.create_handle();
            return process.add_file_handle(handle);
        }

        if (syscall == _SC_close) {
            dbgln("[syscall_handler] _SC_close not implemented");
            return 0;
        }

        if (syscall == _SC_fstat) {
            i32 fd = arg1.fd();
            UserlandFileInfo *statbuf = arg2.pointer<UserlandFileInfo>();

            auto& handle = process.get_file_handle(fd);
            auto& file = handle.file();

            // FIXME: We need some sensible values here
            statbuf->st_dev = FileSystemId::Invalid;
            statbuf->st_rdev = 0xdead;
            statbuf->st_size = 0xdead;
            statbuf->st_blksize = 0xdead;
            statbuf->st_blocks = 0xdead;

            statbuf->st_ino = file.m_ino;
            statbuf->st_mode = file.m_mode;
            statbuf->st_uid = file.m_owning_user;
            statbuf->st_gid = file.m_owning_group;

            return 0;
        }

        if (syscall == _SC_stat) {
            Path path = arg1.pointer<const char>();
            UserlandFileInfo *statbuf = arg2.pointer<UserlandFileInfo>();

            if (!path.is_absolute())
                path = process.m_working_directory / path;

            auto& file = FileSystem::lookup(path);

            // FIXME: We need some sensible values here
            statbuf->st_dev = FileSystemId::Invalid;
            statbuf->st_rdev = 0xdead;
            statbuf->st_size = 0xdead;
            statbuf->st_blksize = 0xdead;
            statbuf->st_blocks = 0xdead;

            statbuf->st_ino = file.m_ino;
            statbuf->st_mode = file.m_mode;
            statbuf->st_uid = file.m_owning_user;
            statbuf->st_gid = file.m_owning_group;

            return 0;
        }

        // FIXME: This is a huge mess
        if (syscall == _SC_fork) {
            Process forked { String::format("Fork: {}", process.m_name) };
            Thread thread { String::format("Process: {}", forked.m_name), move(forked) };
            thread.m_privileged = true;

            auto& process_executable = process.m_executable.must();

            auto forked_executable = process_executable.clone();
            forked_executable.m_entry = forked_executable.m_readonly_base + (return_address - process_executable.m_readonly_base);
            thread.m_process.must().m_executable = move(forked_executable);

            Scheduler::the().create_thread(move(thread), [return_address] () {
                auto& thread = Scheduler::the().active_thread();
                auto& process = thread.m_process.must();
                auto& executable = process.m_executable.must();

                hand_over_to_forked_executable(executable);
            });

            // FIXME: Implement process-ids
            return 1;
        }

        if (syscall == _SC_wait) {
            int *wstatus = arg1.pointer<int>();

            // FIXME: This implementation is so lazy...
            Scheduler::the().donate_my_remaining_cpu_slice();
            return -EINTR;
        }

        VERIFY_NOT_REACHED();
    }
}
