#pragma once

#include <Std/Singleton.hpp>

#include <Kernel/FileSystem/VirtualFileSystem.hpp>

namespace Kernel
{
    class DeviceFile;
    class DeviceFileHandle;
    class DeviceFileSystem;

    class DeviceFileSystem final
        : public Singleton<DeviceFileSystem>
        , public VirtualFileSystem
    {
    public:
        // FIXME: Looks like I choose the wrong abstractions
        VirtualFile& root() override { VERIFY_NOT_REACHED(); }
        VirtualFile& create_file() override { VERIFY_NOT_REACHED(); }

    private:
        friend Singleton<DeviceFileSystem>;
        DeviceFileSystem();
    };
}
