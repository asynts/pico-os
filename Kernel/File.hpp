#pragma once

#include <Kernel/FileSystem/VirtualFileSystem.hpp>

namespace Kernel {
    class File {
    public:
        explicit File(VirtualDirectoryEntry& info)
            : m_info(info)
        {
        }

    private:
        VirtualDirectoryEntry& m_info;
    };

    class FileHandle {
    public:
        explicit FileHandle(File& file, i32 fd)
            : m_file(file)
            , m_fd(fd)
        {
        }

        i32 fd() const { return m_fd; }

        // FIXME: We need to do this on a process basis
        static i32 generate_fd()
        {
            return next_fd++;
        }

    private:
        static i32 next_fd;

        File& m_file;
        i32 m_fd;
    };
}
