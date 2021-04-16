#pragma once

#include <Std/HashMap.hpp>

#include <Kernel/FileSystem/FileSystem.hpp>
#include <Kernel/DynamicLoader.hpp>

namespace Kernel
{
    using namespace Std;

    class Process {
    public:
        explicit Process(String name, Optional<LoadedExecutable> executable = {})
            : m_name(move(name))
            , m_executable(move(executable))
        {
            m_process_id = m_next_process_id++;

            auto& tty_file = FileSystem::lookup("/dev/tty");

            i32 stdin_fileno = add_file_handle(tty_file.create_handle());
            VERIFY(stdin_fileno == 0);

            i32 stdout_fileno = add_file_handle(tty_file.create_handle());
            VERIFY(stdout_fileno == 1);

            i32 stderr_fileno = add_file_handle(tty_file.create_handle());
            VERIFY(stderr_fileno == 2);
        }

        static Process& active_process();
        static Process& create(StringView name, ElfWrapper);

        i32 add_file_handle(VirtualFileHandle& handle)
        {
            i32 handle_id = m_next_handle_id++;
            m_handles.set(handle_id, &handle);

            return handle_id;
        }

        VirtualFileHandle& get_file_handle(i32 fd)
        {
            return *m_handles.get_opt(fd).must();
        }

        Path m_working_directory = "/";
        String m_name;
        Optional<LoadedExecutable> m_executable;

        i32 sys$read(i32 fd, u8 *buffer, usize count);
        i32 sys$write(i32 fd, const u8 *buffer, usize count);
        i32 sys$open(const char *pathname, u32 flags, u32 mode);
        i32 sys$close(i32 fd);
        i32 sys$fstat(i32 fd, UserlandFileInfo *statbuf);
        i32 sys$fork();
        i32 sys$wait(i32 *status);
        i32 sys$execve(const char *pathname, char **argv, char **envp);
        i32 sys$exit(i32 status);

    private:
        static i32 m_next_process_id;

        HashMap<i32, VirtualFileHandle*> m_handles;
        i32 m_next_handle_id = 0;
        i32 m_process_id;
    };
}
