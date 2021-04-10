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

        VirtualFile& file() override { VERIFY_NOT_REACHED(); }
    };

    class ConsoleFile final
        : public Singleton<ConsoleFile>
        , public VirtualFile
    {
    public:
        VirtualFileHandle& create_handle() override
        {
            return *new ConsoleFileHandle;
        }

    private:
        friend Singleton<ConsoleFile>;
        ConsoleFile();
    };
}
