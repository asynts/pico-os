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

        VirtualFileHandle& create_handle();

        VirtualDirectoryEntry& m_info;
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
