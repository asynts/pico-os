#include <Kernel/Worker.hpp>
#include <Kernel/Scheduler.hpp>

namespace Kernel
{
    Worker::Worker()
    {
        Thread thread { "Kernel::Worker" };

        Scheduler::the().create_thread(move(thread), [this] {
            this->run();
        });
    }

    void Worker::run()
    {
        for (;;) {
        }
    }
}
