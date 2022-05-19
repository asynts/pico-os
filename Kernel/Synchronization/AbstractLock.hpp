#pragma once

#include <Kernel/HandlerMode.hpp>

namespace Kernel
{
    class AbstractLock
    {
    public:
        virtual ~AbstractLock() = default;

        virtual void lock() = 0;
        virtual void unlock() = 0;
    };
}
