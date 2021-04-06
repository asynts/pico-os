#pragma once

#include <Std/HashMap.hpp>

#include <Kernel/FileSystem/FileSystem.hpp>

namespace Kernel
{
    using namespace Std;

    class Process {
    public:
        Process()
        {
            auto& tty_file = FileSystem::lookup("/dev/tty");

            i32 stdin_fileno = add_file_handle(tty_file.create_handle());
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
            m_handles.set(handle_id, &handle);

            return handle_id;
        }

        VirtualFileHandle& get_file_handle(i32 fd)
        {
            return *m_handles.get_opt(fd).must();
        }

        Path m_working_directory = "/";

    private:
        HashMap<i32, VirtualFileHandle*> m_handles;
        i32 m_next_handle_id = 0;
    };
}
