#include <Kernel/FileSystem/FlashFileSystem.hpp>
#include <Kernel/FileSystem/FileSystem.hpp>

// FIXME: Remove redundant fields
// FIXME: Move into interface
struct FlashFileInfo {
    u32 m_ino;
    u32 m_device;
    u32 m_mode;
    u32 m_size;
    u32 m_devno;
    u8 *m_data;
};
struct FlashDirectoryEntryInfo {
    char m_name[252];
    FlashFileInfo *m_info;
};

extern "C" FlashFileInfo __flash_root;

namespace Kernel
{
    VirtualFile& FlashFileSystem::create_file() { return *new FlashFile; }
    VirtualFile& FlashFileSystem::root() { return *m_root; }

    FlashFileSystem::FlashFileSystem()
    {
        // FIXME: Figure this out
        m_root = nullptr;
    }

    VirtualFileHandle& FlashFile::create_handle()
    {
        auto& handle = *new FlashFileHandle;
        handle.m_file = this;
        return handle;
    }
}
