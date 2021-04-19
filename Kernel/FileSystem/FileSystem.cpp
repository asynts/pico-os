#include <Kernel/FileSystem/FileSystem.hpp>
#include <Kernel/FileSystem/MemoryFileSystem.hpp>
#include <Kernel/Interface/System.hpp>

namespace Kernel::FileSystem
{
    VirtualFile& lookup(Path path)
    {
        VERIFY(path.is_absolute());

        VirtualFile *file = &MemoryFileSystem::the().root();

        for (auto& component : path.components()) {
            auto *directory = dynamic_cast<VirtualDirectory*>(file);
            ASSERT(directory != nullptr);

            file = directory->m_entries.get_opt(component).must();
        }

        ASSERT(file != nullptr);
        return *file;
    }

    KernelResult<VirtualFile*> try_lookup(Path path)
    {
        VERIFY(path.is_absolute());

        VirtualFile *file = &MemoryFileSystem::the().root();

        for (auto& component : path.components()) {
            auto *directory = dynamic_cast<VirtualDirectory*>(file);

            if (directory == nullptr)
                return ENOTDIR;

            auto file_opt = directory->m_entries.get_opt(component);

            if (!file_opt.is_valid())
                return ENOENT;

            file = file_opt.value();
        }

        ASSERT(file != nullptr);
        return file;
    }

    static HashMap<u32, VirtualFile*> devices;

    void add_device(u32 device, VirtualFile& file)
    {
        devices.set(device, &file);
    }

    VirtualFileHandle& create_handle_for_device(u32 device)
    {
        return devices.get_opt(device).must()->create_handle();
    }
}
