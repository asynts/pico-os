#include <Kernel/VirtualFileSystem.hpp>

namespace Kernel
{
    void VirtualDirectoryEntry::add_entry(StringView name, FileInfo& info, bool keep)
    {
        auto& dentry = m_fs->create_empty_dentry();

        dentry.m_info = &info;
        dentry.m_keep |= keep;
        dentry.m_loaded = false;
        dentry.m_name = name;
        dentry.m_entries.append(".", &dentry);
        dentry.m_entries.append("..", this);

        m_entries.append(name, &dentry);
    }
}
