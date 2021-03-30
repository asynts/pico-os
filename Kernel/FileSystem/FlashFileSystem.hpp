#pragma once

#include <Std/Singleton.hpp>

#include <Kernel/FileSystem/VirtualFileSystem.hpp>

namespace Kernel
{
    using namespace Std;

    class FlashFileSystem;
    class FlashFile;
    class FlashDirectoryEntry;
    class FlashFileHandle;

    class FlashFileSystem final
        : public Singleton<FlashFileSystem>
        , public VirtualFileSystem
    {
    public:
        VirtualFile& create_file() override;
        VirtualFileHandle& create_file_handle() override;
        VirtualDirectoryEntry& create_directory_entry() override;
    };

    class FlashFile final
        : public VirtualFile
    {
    public:
        VirtualFileSystem& filesystem() override { return FlashFileSystem::the(); }
    };

    class FlashDirectoryEntry final
        : public VirtualDirectoryEntry
    {
    public:
        VirtualFile& file() override { return *m_file; }

        FlashFile *m_file;
    };

    class FlashFileHandle final
        : public VirtualFileHandle
    {
    public:
        VirtualFile& file() override { return *m_file; }

        FlashFile *m_file;
    };
}
