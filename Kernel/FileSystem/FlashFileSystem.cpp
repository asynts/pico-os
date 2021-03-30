#include <Kernel/FileSystem/FlashFileSystem.hpp>

namespace Kernel
{
    VirtualFile& FlashFileSystem::create_file() { return *new FlashFile; }
    VirtualFileHandle& FlashFileSystem::create_file_handle() { return *new FlashFileHandle; }
    VirtualDirectoryEntry& FlashFileSystem::create_directory_entry() { return *new FlashDirectoryEntry; }
}
