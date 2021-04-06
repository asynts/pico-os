#include <Kernel/FileSystem/FileSystem.hpp>
#include <Kernel/FileSystem/MemoryFileSystem.hpp>

namespace Kernel::FileSystem
{
    VirtualFile& lookup(Path path)
    {
        VERIFY(path.is_absolute());

        VirtualFile *file = &MemoryFileSystem::the().root();

        for (auto& component : path.components()) {
            // FIXME: Figure this out
            ASSERT_NOT_REACHED();
        }

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
