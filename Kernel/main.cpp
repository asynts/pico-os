#include <Std/Forward.hpp>
#include <Std/Format.hpp>
#include <Kernel/Synchronization/SoftwareSpinLock.hpp>
#include <Kernel/Synchronization/HardwareSpinLock.hpp>
#include <Kernel/KernelMutex.hpp>

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

extern "C" u8 __end__[];
extern "C" u8 __HeapLimit[];

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

    // Hardware Spin Locks (RP2040 SIO Base 0xd0000000 + 0x100 offset)
    static HardwareSpinLock s_sw_spinlock_hw_lock((volatile u32*)0xd0000100); // Lock 0
    static HardwareSpinLock s_malloc_hw_lock((volatile u32*)0xd0000104);      // Lock 1
    static HardwareSpinLock s_page_hw_lock((volatile u32*)0xd0000108);        // Lock 2
    static HardwareSpinLock s_dbgln_hw_lock((volatile u32*)0xd000010c);       // Lock 3

    // Setup basic systems and run 'boot_with_scheduler' in a new thread
    void boot()
    {
        // Initialize Synchronization Primitives
        SoftwareSpinLock::initialize(s_sw_spinlock_hw_lock);
        malloc_mutex.initialize(s_malloc_hw_lock);
        page_allocator_mutex.initialize(s_page_hw_lock);
        dbgln_mutex.initialize(s_dbgln_hw_lock);

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

        dbgln("__HeapLimit={} __end__={}", __HeapLimit, __end__);

        dbgln("[main] Creating /example.txt");
        auto& example_file = *new Kernel::MemoryFile;
        auto& example_handle = example_file.create_handle();
        example_handle.write({ (const u8*)"Hello, world!\n", 14 });

        auto& root_file = Kernel::FileSystem::lookup("/");
        dynamic_cast<Kernel::VirtualDirectory&>(root_file).m_entries.set("example.txt", &example_file);

        Kernel::SystemHandler::initialize();

        // debug_page_allocator = true;

        create_shell_process();
    }
}

int main()
{
    Kernel::boot();
}
