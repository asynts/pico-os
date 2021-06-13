#pragma once

#include <Std/Singleton.hpp>

#include <Kernel/Forward.hpp>
#include <Kernel/Threads/Scheduler.hpp>

namespace Kernel
{
    // FIXME: This class is very poorly named
    class Worker : public Singleton<Worker> {
    private:
        Thread m_thread;

        friend Singleton<Worker>;
        Worker();

        void work();
    };
}
