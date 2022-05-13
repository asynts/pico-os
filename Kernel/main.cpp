#include <Std/Forward.hpp>
#include <Std/Format.hpp>

#include <Kernel/Loader.hpp>
#include <Kernel/ConsoleDevice.hpp>
#include <Kernel/FileSystem/MemoryFileSystem.hpp>
#include <Kernel/FileSystem/FlashFileSystem.hpp>
#include <Kernel/FileSystem/DeviceFileSystem.hpp>
#include <Kernel/Process.hpp>
#include <Kernel/GlobalMemoryAllocator.hpp>
#include <Kernel/Threads/Scheduler.hpp>
#include <Kernel/ConsoleDevice.hpp>
#include <Kernel/Interrupt/UART.hpp>
#include <Kernel/PageAllocator.hpp>
#include <Kernel/SystemHandler.hpp>

#include <hardware/structs/mpu.h>

#include <Kernel/MPU.hpp>

// FIXME: Remove 'Kernel::' prefixes

namespace Kernel
{
    // FIXME: Clean this up
    void create_shell_process()
    {
        auto& shell_file = dynamic_cast<Kernel::FlashFile&>(Kernel::FileSystem::lookup("/bin/Shell.elf"));
        Kernel::ElfWrapper elf { shell_file.m_data.data(), "Userland/Shell.1.elf" };
        Kernel::Process::create("/bin/Shell.elf", move(elf));
    }

    void boot_with_scheduler();

    // Setup basic systems and run 'boot_with_scheduler' in a new thread
    void boot()
    {
        Kernel::PageAllocator::initialize();
        Kernel::PageAllocator::the().set_mutex_enabled(false);

        Kernel::GlobalMemoryAllocator::initialize();
        Kernel::GlobalMemoryAllocator::the().set_mutex_enabled(false);

        Kernel::Interrupt::UART::initialize();
        Kernel::ConsoleFile::initialize();

        dbgln("\e[0;1mBOOT\e[0m");

        auto thread = Kernel::Thread::construct("Kernel (boot_with_scheduler)");
        thread->setup_context(boot_with_scheduler);
        thread->m_privileged = true;

        Kernel::Scheduler::initialize(move(thread));
        Kernel::Scheduler::the().loop();
    }

    void boot_with_scheduler()
    {
        Kernel::FlashFileSystem::initialize();
        Kernel::MemoryFileSystem::initialize();
        Kernel::DeviceFileSystem::initialize();

        dbgln("[main] Creating /example.txt");
        auto& example_file = *new Kernel::MemoryFile;
        auto& example_handle = example_file.create_handle();
        example_handle.write({ (const u8*)"Hello, world!\n", 14 });

        auto& root_file = Kernel::FileSystem::lookup("/");
        dynamic_cast<Kernel::VirtualDirectory&>(root_file).m_entries.set("example.txt", &example_file);

        Kernel::SystemHandler::initialize();

        create_shell_process();
    }
}

int main()
{
    Kernel::boot();
}
