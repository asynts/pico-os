#include <Kernel/FileSystem/FlashFileSystem.hpp>
#include <Kernel/FileSystem/MemoryFileSystem.hpp>

namespace Kernel {
    extern "C" FileInfo __flash_root;

    FlashDirectoryEntry::FlashDirectoryEntry()
    {
        m_fs = &FlashFileSystem::the();
        m_loaded = false;
    }

    FlashFileSystem::FlashFileSystem()
    {
        m_root = &MemoryFileSystem::the().root().add_entry("bin", __flash_root, true);
    }
}
