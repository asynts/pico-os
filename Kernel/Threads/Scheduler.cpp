#include <Kernel/Threads/Scheduler.hpp>

namespace Kernel
{
    Scheduler::Scheduler()
        : m_default_thread("Default Thread (Core 0)")
    {
        m_default_thread.setup_context([] {
            for (;;) {
                asm volatile ("wfi");
            }
        });
        m_queued_threads.enqueue(&m_default_thread);

        // We need to setup interrupts and similar stuff
        FIXME();
    }
}
