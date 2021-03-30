#pragma once

#include <Std/Map.hpp>

#include <Kernel/FileSystem/FileSystem.hpp>

namespace Kernel
{
    using namespace Std;

    class Process {
    public:
        Process()
        {
            auto& tty_dentry = FileSystem::lookup("/dev/tty");

            i32 stdin_fileno = add_file_handle(tty_dentry.file().create_handle());
            VERIFY(stdin_fileno == 0);

            i32 stdout_fileno = add_file_handle(tty_file.create_handle());
            VERIFY(stdout_fileno == 1);

            i32 stderr_fileno = add_file_handle(tty_file.create_handle());
            VERIFY(stderr_fileno == 2);
        }

        static Process& current();

        i32 add_file_handle(VirtualFileHandle& handle)
        {
            i32 handle_id = m_next_handle_id++;
            m_handles.append(handle_id, &handle);

            return handle_id;
        }

        VirtualFileHandle& get_file_handle(i32 fd)
        {
            return *m_handles.lookup(fd).must();
        }

        Path m_working_directory = "/";

    private:
        Map<i32, VirtualFileHandle*> m_handles;
        i32 m_next_handle_id = 0;
    };
}
