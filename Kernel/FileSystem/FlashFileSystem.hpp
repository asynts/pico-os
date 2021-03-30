#pragma once

#include <Std/Singleton.hpp>
#include <Std/Span.hpp>

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
        VirtualDirectoryEntry& root() override { return *m_root; }

        VirtualFile& create_file() override;
        VirtualFileHandle& create_file_handle() override;
        VirtualDirectoryEntry& create_directory_entry() override;

    private:
        friend Singleton<FlashFileSystem>;
        FlashFileSystem();

        FlashDirectoryEntry *m_root;
    };

    class FlashFile final
        : public VirtualFile
    {
    public:
        VirtualFileSystem& filesystem() override { return FlashFileSystem::the(); }

        ReadonlyBytes m_data;
    };

    class FlashDirectoryEntry final
        : public VirtualDirectoryEntry
    {
    public:
        VirtualFile& file() override { return *m_file; }

        void load() override
        {
            NOT_IMPLEMENTED();
        }

        FlashFile *m_file;
        bool m_loaded = false;
    };

    class FlashFileHandle final
        : public VirtualFileHandle
    {
    public:
        VirtualFile& file() override { return *m_file; }

        FlashFile *m_file;
    };
}
