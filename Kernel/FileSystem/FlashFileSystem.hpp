#pragma once

#include <Std/Singleton.hpp>
#include <Std/Span.hpp>

#include <Kernel/FileSystem/VirtualFileSystem.hpp>

namespace Kernel
{
    using namespace Std;

    class FlashFileSystem;
    class FlashFile;
    class FlashFileHandle;

    class FlashFileSystem final
        : public Singleton<FlashFileSystem>
        , public VirtualFileSystem
    {
    public:
        VirtualFile& root() override;

        VirtualFile& create_file() override;

    private:
        friend Singleton<FlashFileSystem>;
        FlashFileSystem();

        FlashFile *m_root;
    };

    class FlashFile final : public VirtualFile {
    public:
        VirtualFileSystem& filesystem() override { return FlashFileSystem::the(); }

        VirtualFileHandle& create_handle() override;

        ReadonlyBytes m_data;
    };

    class FlashFileHandle final : public VirtualFileHandle {
    public:
        VirtualFile& file() override { return *m_file; }

        KernelResult<usize> read(Bytes bytes) override
        {
            usize nread = m_file->m_data.slice(m_offset).copy_trimmed_to(bytes);
            m_offset += nread;
            return nread;
        }

        KernelResult<usize> write(ReadonlyBytes bytes) override
        {
            VERIFY_NOT_REACHED();
        }

        FlashFile *m_file;
        usize m_offset = 0;
    };
}
