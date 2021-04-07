#pragma once

#include <Std/HashMap.hpp>
#include <Std/String.hpp>

#include <Kernel/Result.hpp>

namespace Kernel
{
    using namespace Std;

    class VirtualFile;
    class VirtualFileHandle;
    class VirtualFileSystem;

    enum class ModeFlags : u32 {
        Format    = 0b1111,
        Directory = 0b0001,
        Device    = 0b0010,
        Regular   = 0b0011,
    };

    inline ModeFlags operator&(ModeFlags lhs, ModeFlags rhs)
    {
        return static_cast<ModeFlags>(static_cast<u32>(lhs) & static_cast<u32>(rhs));
    }

    class VirtualFileSystem {
    public:
        virtual ~VirtualFileSystem() = default;

        virtual VirtualFile& root() = 0;
    };

    class VirtualFile {
    public:
        virtual ~VirtualFile() = default;

        u32 m_ino;
        ModeFlags m_mode;

        virtual VirtualFileHandle& create_handle() = 0;
    };

    class VirtualDirectory : public VirtualFile {
    public:
        virtual ~VirtualDirectory() = default;

        HashMap<String, VirtualFile*> m_entries;
    };

    class VirtualFileHandle {
    public:
        virtual VirtualFile& file() = 0;

        virtual KernelResult<usize> read(Bytes) = 0;
        virtual KernelResult<usize> write(ReadonlyBytes) = 0;
    };
}
