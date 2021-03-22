#include <Kernel/Process.hpp>

namespace Kernel
{
    // FIXME: Deal with multiple processes
    Process& Process::current()
    {
        static Process process;
        return process;
    }
}
