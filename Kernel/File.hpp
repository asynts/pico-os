#pragma once

#include <Kernel/FileSystem/VirtualFileSystem.hpp>
#include <Kernel/ConsoleDevice.hpp>

namespace Kernel
{
    class VirtualFileHandle;

    class File {
    public:
        explicit File(VirtualDirectoryEntry& info)
            : m_info(info)
        {
        }

        // FIXME: This should go with the inode?
        VirtualFileHandle& create_handle();

        VirtualDirectoryEntry& m_info;
        usize m_offset = 0;
    };

    class VirtualFileHandle {
    public:
        virtual ~VirtualFileHandle() = default;

        explicit VirtualFileHandle(File& file)
            : m_file(file)
        {
        }

        virtual usize read(Bytes) = 0;
        virtual usize write(ReadonlyBytes) = 0;

    protected:
        File& m_file;
    };
}
