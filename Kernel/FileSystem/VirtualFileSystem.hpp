#pragma once

#include <Std/HashMap.hpp>
#include <Std/String.hpp>

#include <Kernel/Result.hpp>
#include <Kernel/Interface/Types.hpp>

namespace Kernel
{
    using namespace Std;

    class VirtualFile;
    class VirtualFileHandle;
    class VirtualFileSystem;

    class VirtualFileSystem {
    public:
        virtual ~VirtualFileSystem() = default;

        virtual VirtualFile& root() = 0;
    };

    class VirtualFile {
    public:
        virtual ~VirtualFile() = default;

        VirtualFile()
        {
            m_ino = 0;
            m_mode = ModeFlags::Invalid;
            m_owning_user = 0;
            m_owning_group = 0;
        }

        u32 m_ino;
        ModeFlags m_mode;
        u32 m_owning_user;
        u32 m_owning_group;

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
