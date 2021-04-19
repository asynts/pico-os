#pragma once

#include <Std/Path.hpp>

#include <Kernel/FileSystem/VirtualFileSystem.hpp>

namespace Kernel::FileSystem
{
    // FIXME: Remove this function, try_lookup should take this name
    VirtualFile& lookup(Path);

    KernelResult<VirtualFile*> try_lookup(Path);

    void add_device(u32 device, VirtualFile&);
    VirtualFileHandle& create_handle_for_device(u32 device);
}
