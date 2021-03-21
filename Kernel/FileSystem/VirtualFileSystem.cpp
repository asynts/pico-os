#include <Kernel/FileSystem/VirtualFileSystem.hpp>
#include <Kernel/FileSystem/MemoryFileSystem.hpp>
#include <Kernel/FileSystem/FlashFileSystem.hpp>

namespace Kernel
{
    // FIXME: Do this properly
    static VirtualFileSystem& filesystem_by_device(u32 device)
    {
        if (device == RAM_DEVICE_ID)
            return MemoryFileSystem::the();
        if (device == FLASH_DEVICE_ID)
            return FlashFileSystem::the();

        assert(false);
    }

    VirtualDirectoryEntry& VirtualDirectoryEntry::add_entry(StringView name, FileInfo& info, bool keep)
    {
        auto& dentry = filesystem_by_device(info.m_device).create_empty_dentry();

        dentry.m_info = &info;
        dentry.m_keep |= keep;
        dentry.m_loaded = false;
        dentry.m_name = name;
        dentry.m_entries.append(".", &dentry);
        dentry.m_entries.append("..", this);

        m_entries.append(name, &dentry);
        return dentry;
    }
}
