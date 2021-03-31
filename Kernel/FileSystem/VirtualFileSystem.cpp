#include <Kernel/FileSystem/VirtualFileSystem.hpp>

namespace Kernel
{
    VirtualFile& VirtualFileSystem::create_regular()
    {
        auto& file = create_file();
        file.m_mode = ModeFlags::Regular;
        file.m_device = 0;
        return file;
    }
}
