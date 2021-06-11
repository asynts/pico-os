#pragma once

#include <Std/Singleton.hpp>

namespace Kernel
{
    using namespace Std;

    // Interrupt handlers can delegate work to this worker such that interrupts
    // can be unblocked sooner
    class Worker : public Singleton<Worker> {
    private:
        friend Singleton<Worker>;
        Worker();

        void run();
    };
}
