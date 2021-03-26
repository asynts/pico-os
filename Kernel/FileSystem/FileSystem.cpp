#include <Std/Map.hpp>

#include <Kernel/FileSystem/FileSystem.hpp>
#include <Kernel/FileSystem/MemoryFileSystem.hpp>
#include <Kernel/File.hpp>

namespace Kernel::FileSystem
{
    class FileSystemRegistry : public Singleton<FileSystemRegistry> {
    public:
        void register_filesystem(u32 device)
        {
            map.append(device, {});
        }
        void register_info(u32 device, u32 ino, VirtualFile& info)
        {
            auto& table = map.lookup(device).must();
            table.append(ino, &info);
        }

        Optional<VirtualFile*> lookup(u32 device, u32 ino)
        {
            auto table = map.lookup(device);
            if (!table.is_valid())
                return {};

            return table.value().lookup(ino);
        }

    private:
        Map<u32, Map<u32, VirtualFile*>> map;
    };

    void add_filesystem(u32 device)
    {
        FileSystemRegistry::the().register_filesystem(device);
    }

    VirtualFile& file_from_info(FileInfo& info)
    {
        if (auto file = FileSystemRegistry::the().lookup(info.m_device, info.m_ino); file.is_valid())
            return *file.value();

        // FIXME: Two different filesystems could have device files referencing the same device
        if ((info.m_mode & S_IFMT) == S_IFDEV) {
            auto& file = *new DeviceFile { info };
            FileSystemRegistry::the().register_info(info.m_device, info.m_ino, file);
            return file;
        }

        if (info.m_device == RAM_DEVICE_ID) {
            auto& file = *new RamFile { info };
            FileSystemRegistry::the().register_info(info.m_device, info.m_ino, file);
            return file;
        }
        if (info.m_device == FLASH_DEVICE_ID) {
            auto& file = *new FlashFile { info };
            FileSystemRegistry::the().register_info(info.m_device, info.m_ino, file);
            return file;
        }

        VERIFY_NOT_REACHED();
    }

    VirtualDirectoryEntry& lookup_dentry(StringView path)
    {
        return MemoryFileSystem::the().lookup_path(path);
    }

    VirtualFile& lookup_file(StringView path)
    {
        return file_from_info(*MemoryFileSystem::the().lookup_path(path).m_info);
    }
}
