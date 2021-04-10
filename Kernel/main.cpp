#include <Std/Forward.hpp>
#include <Std/Format.hpp>

#include <Kernel/DynamicLoader.hpp>
#include <Kernel/ConsoleDevice.hpp>
#include <Kernel/FileSystem/MemoryFileSystem.hpp>
#include <Kernel/FileSystem/FlashFileSystem.hpp>
#include <Kernel/FileSystem/DeviceFileSystem.hpp>
#include <Kernel/Process.hpp>
#include <Kernel/GlobalMemoryAllocator.hpp>
#include <Kernel/Scheduler.hpp>
#include <Kernel/ConsoleDevice.hpp>

extern "C" {
    extern u8 __fs_start[];
    extern u8 __fs_end[];
}

void load_and_execute_shell()
{
    auto& shell_file = dynamic_cast<Kernel::FlashFile&>(Kernel::FileSystem::lookup("/bin/Shell.elf"));

    ElfWrapper elf { shell_file.m_data.data() };
    LoadedExecutable executable = load_executable_into_memory(elf);

    // FIXME: Do this properly
    Kernel::Process::current();

    dbgln("[load_and_execute_shell] Switching to shell process");

    asm volatile(
        "movs r0, #0;"
        "msr psp, r0;"
        "isb;"
        "movs r0, #0b11;"
        "msr control, r0;"
        "isb;"
        "mov r0, %1;"
        "mov sb, %2;"
        "blx %0;"
        :
        : "r"(executable.m_entry), "r"(executable.m_stack_base + executable.m_stack_size), "r"(executable.m_writable_base)
        : "r0");

    VERIFY_NOT_REACHED();
}

[[noreturn]]
void example_kernel_thread()
{
    dbgln("[example_kernel_thread] Hello, world!");
    for(;;)
        asm volatile("wfi");
}

int main()
{
    Kernel::ConsoleFile::the();

    dbgln("\e[0;1mBOOT\e[0m");

    Kernel::GlobalMemoryAllocator::the();
    Kernel::Scheduler::the();

    Kernel::MemoryFileSystem::the();
    Kernel::FlashFileSystem::the();
    Kernel::DeviceFileSystem::the();

    dbgln("[main] Creating /example.txt");
    auto& example_file = *new Kernel::MemoryFile;
    auto& example_handle = example_file.create_handle();
    example_handle.write({ (const u8*)"Hello, world!\n", 14 });

    auto& root_file = Kernel::FileSystem::lookup("/");
    dynamic_cast<Kernel::VirtualDirectory&>(root_file).m_entries.set("example.txt", &example_file);

    Kernel::Scheduler::the().create_thread("Kernel: example", example_kernel_thread);

    Kernel::Scheduler::the().create_thread("Kernel: lambda without capture", [] {
        dbgln("Hello, world from lambda '{}'", __PRETTY_FUNCTION__);
        for(;;)
            asm volatile("wfi");
    });

    Kernel::Scheduler::the().create_thread("Kernel: lambda with captures", [x = 42] {
        dbgln("I got x={} in the lambda!", x);
        for(;;)
            asm volatile("wfi");
    });

    Kernel::Scheduler::the().loop();

    load_and_execute_shell();

    for(;;)
        asm volatile("wfi");
}
