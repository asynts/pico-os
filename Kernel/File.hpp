#pragma once

#include <Kernel/FileSystem/VirtualFileSystem.hpp>
#include <Kernel/ConsoleDevice.hpp>

namespace Kernel
{
    class VirtualFileHandle;

    class VirtualFile {
    public:
        virtual ~VirtualFile() = default;

        explicit VirtualFile(FileInfo& info)
            : m_info(info)
        {
        }

        virtual VirtualFileHandle& create_handle() = 0;

        FileInfo& m_info;
    };

    class VirtualFileHandle {
    public:
        virtual ~VirtualFileHandle() = default;

        virtual usize read(Bytes) = 0;
        virtual usize write(ReadonlyBytes) = 0;
    };
}
