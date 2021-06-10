#pragma once

#include <Std/Span.hpp>
#include <Std/Singleton.hpp>

#include <Kernel/FileSystem/VirtualFileSystem.hpp>

namespace Kernel {
    using namespace Std;

    class ConsoleFileHandle final : public VirtualFileHandle
    {
    public:
        KernelResult<usize> read(Bytes bytes) override;
        KernelResult<usize> write(ReadonlyBytes bytes) override;

        VirtualFile& file() override;
    };

    class ConsoleFile final
        : public Singleton<ConsoleFile>
        , public VirtualFile
    {
    public:
        VirtualFileHandle& create_handle_impl() override
        {
            return *new ConsoleFileHandle;
        }

        void truncate() override
        {
            VERIFY_NOT_REACHED();
        }

    private:
        friend Singleton<ConsoleFile>;
        ConsoleFile();
    };
}
