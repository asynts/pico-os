#pragma once

#include <Kernel/File.hpp>

namespace Kernel::FileSystem
{
    void add_filesystem(u32 device);

    VirtualFile& file_from_info(FileInfo&);

    VirtualDirectoryEntry& lookup_dentry(StringView path);
    VirtualFile& lookup_file(Path path);
}
