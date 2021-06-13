#include <Kernel/Worker.hpp>
#include <Kernel/HandlerMode.hpp>

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

        for (;;) {
            VERIFY(is_executing_privileged());

            for (Thread *thread : Scheduler::the().m_blocked_threads.iter()) {
                // FIXME
            }

            asm volatile("wfi");
        }
    }
}
