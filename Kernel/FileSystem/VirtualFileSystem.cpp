#include <Kernel/FileSystem/VirtualFileSystem.hpp>
#include <Kernel/FileSystem/DeviceFileSystem.hpp>

namespace Kernel
{
    VirtualFileHandle& VirtualFile::create_handle()
    {
        if ((m_mode & ModeFlags::Format) == ModeFlags::Device)
            return DeviceFileSystem::the().create_device_handle(m_device_id);
        else
            return create_handle_impl();
    }
}
