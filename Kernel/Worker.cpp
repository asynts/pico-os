#include <Kernel/Worker.hpp>
#include <Kernel/HandlerMode.hpp>
#include <Kernel/Interface/System.hpp>
#include <Kernel/ConsoleDevice.hpp>

namespace Kernel
{
    Worker::Worker()
        : m_thread("Worker")
    {
        m_thread.m_privileged = true;

        m_thread.setup_context([this]() mutable { this->work(); });

        Scheduler::the().add_thread(m_thread);
    }

    void Worker::work()
    {
        VERIFY(is_executing_privileged());

        dbgln("[Worker::work]");

        auto& scheduler = Scheduler::the();

        for (;;) {
            VERIFY(is_executing_privileged());

            if (scheduler.m_blocked_threads.size() > 0) {
                Thread *thread = scheduler.m_blocked_threads.dequeue();

                dbgln("[Worker::work] Working on '{}' ({})", thread->m_name, thread);

                FIXME_ASSERT(thread->m_running_system_call.is_valid());

                auto& info = thread->m_running_system_call.must();

                Optional<u32> return_value;
                if (info.m_type == _SC_write) {
                    FIXME_ASSERT(info.m_data.m_write.m_fd == STDOUT_FILENO);

                    ConsoleFileHandle handle;

                    return_value = handle.write(info.m_data.m_write.m_buffer).must();
                } else  if (info.m_type == _SC_read) {
                    FIXME_ASSERT(info.m_data.m_read.m_fd == STDIN_FILENO);

                    ConsoleFileHandle handle;
                    return_value = handle.read(info.m_data.m_read.m_buffer).must();
                } else {
                    FIXME();
                }

                thread->m_stashed_context.must()->r0.m_storage = return_value.must();
                thread->m_blocked = false;

                scheduler.m_queued_threads.enqueue(thread);
            }

            asm volatile("wfi");
        }
    }
}
