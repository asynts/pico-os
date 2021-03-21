#include <Kernel/MemoryFileSystem.hpp>

namespace Kernel
{
    MemoryDirectoryEntry::MemoryDirectoryEntry()
    {
        m_fs = &MemoryFileSystem::the();
        m_keep = true;
    }
}
