#pragma once

#include <Kernel/Synchronization/SoftwareMutex.hpp>

namespace Kernel
{
    // KernelMutex is now just a SoftwareMutex
    using KernelMutex = SoftwareMutex;

    extern KernelMutex dbgln_mutex;
    extern KernelMutex malloc_mutex;
    extern KernelMutex page_allocator_mutex;
}
