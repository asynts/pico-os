#pragma once

#include <Std/HashMap.hpp>
#include <Std/CircularQueue.hpp>
#include <Std/RefPtr.hpp>

#include <Kernel/FileSystem/FileSystem.hpp>
#include <Kernel/Loader.hpp>

namespace Kernel
{
    class Process : public RefCounted<Process> {
    public:
        struct TerminatedProcess {
            i32 m_process_id;
            i32 m_status;
        };

        static Process& active();

        static Process& create(StringView name, ElfWrapper);
        static Process& create(StringView name, ElfWrapper, const Vector<ImmutableString>& arguments, const Vector<ImmutableString>& variables);

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
        ImmutableString m_name;
        Optional<LoadedExecutable> m_executable;

        Process *m_parent = nullptr;
        i32 m_process_id;
        CircularQueue<TerminatedProcess, 8> m_terminated_children;

    private:
        static inline i32 m_next_process_id = 0;

        HashMap<i32, VirtualFileHandle*> m_handles;
        i32 m_next_handle_id = 0;

        friend RefCounted<Process>;
        explicit Process(ImmutableString name, Optional<LoadedExecutable> executable = {})
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
    };
}
