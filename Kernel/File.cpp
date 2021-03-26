#include <Kernel/File.hpp>

namespace Kernel
{
    VirtualFileHandle& RamFile::create_handle()
    {
        return *new RamFileHandle { *this };
    }

    VirtualFileHandle& FlashFile::create_handle()
    {
        return *new FlashFileHandle { *this };
    }

    VirtualFileHandle& DeviceFile::create_handle()
    {
        return *new DeviceFileHandle { *this };
    }
}
