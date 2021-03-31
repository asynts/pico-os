#pragma once

#include <Std/Path.hpp>

#include <Kernel/FileSystem/VirtualFileSystem.hpp>

namespace Kernel::FileSystem
{
    VirtualDirectoryEntry& lookup(Path);

    void add_device(u32 device, VirtualFile&);
    VirtualFileHandle& create_handle_for_device(u32 device);
}
