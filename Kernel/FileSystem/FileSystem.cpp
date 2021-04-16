#include <Kernel/FileSystem/FileSystem.hpp>
#include <Kernel/FileSystem/MemoryFileSystem.hpp>

namespace Kernel::FileSystem
{
    VirtualFile& lookup(Path path)
    {
        dbgln("[FileSystem::lookup] path={}", path);

        VERIFY(path.is_absolute());

        VirtualFile *file = &MemoryFileSystem::the().root();
        dbgln("[FileSystem::lookup] file={} processed=/", file);

        for (auto& component : path.components()) {
            auto *directory = dynamic_cast<VirtualDirectory*>(file);
            ASSERT(directory != nullptr);

            file = directory->m_entries.get_opt(component).must();
            dbgln("[FileSystem::lookup] file={} processed={}", file, component);
        }

        ASSERT(file != nullptr);
        return *file;
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
