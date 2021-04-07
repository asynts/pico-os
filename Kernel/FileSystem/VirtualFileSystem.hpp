#pragma once

#include <Std/HashMap.hpp>
#include <Std/String.hpp>

#include <Kernel/Result.hpp>
#include <Kernel/Interface/stat.h>

namespace Kernel
{
    using namespace Std;

    class VirtualFile;
    class VirtualFileHandle;
    class VirtualFileSystem;

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
