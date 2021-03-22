#pragma once

#include <Std/Map.hpp>

#include <Kernel/File.hpp>
#include <Kernel/FileSystem/MemoryFileSystem.hpp>

namespace Kernel
{
    using namespace Std;

    class Process {
    public:
        Process()
        {
            auto& stdin_file = *new File { MemoryFileSystem::the().lookup_path("/dev/tty") };
            i32 stdin_fileno = add_file_handle(stdin_file.create_handle());
            assert(stdin_fileno == 0);

            auto& stdout_file = *new File { MemoryFileSystem::the().lookup_path("/dev/tty") };
            i32 stdout_fileno = add_file_handle(stdout_file.create_handle());
            assert(stdout_fileno == 1);
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

    private:
        Map<i32, VirtualFileHandle*> m_handles;
        i32 m_next_handle_id = 0;
    };
}
