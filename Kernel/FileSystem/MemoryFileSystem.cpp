#include <Kernel/FileSystem/MemoryFileSystem.hpp>

namespace Kernel
{
    VirtualFile& MemoryFileSystem::create_file() { return *new MemoryFile; }
    VirtualFileHandle& MemoryFileSystem::create_file_handle() { return *new MemoryFileHandle; }
    VirtualDirectoryEntry& MemoryFileSystem::create_directory_entry() { return *new MemoryDirectoryEntry; }
}
