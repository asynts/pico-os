#include <pico/sync.h>
#include <hardware/uart.h>
#include <hardware/gpio.h>

#include <Std/Forward.hpp>
#include <Std/Format.hpp>
#include <Kernel/DynamicLoader.hpp>
#include <Kernel/ConsoleDevice.hpp>
#include <Kernel/FileSystem/MemoryFileSystem.hpp>
#include <Kernel/FileSystem/FlashFileSystem.hpp>
#include <Kernel/FileSystem/DeviceFileSystem.hpp>
#include <Kernel/Process.hpp>
#include <Kernel/MemoryAllocator.hpp>

#include <pico/stdio.h>

extern "C" {
    extern u8 __fs_start[];
    extern u8 __fs_end[];
}

void load_and_execute_shell()
{
    auto& shell_dentry = Kernel::FileSystem::lookup("/bin/Shell.elf");
    auto& shell_file = dynamic_cast<Kernel::FlashFile&>(shell_dentry.file());

    ElfWrapper elf { shell_file.m_data.data() };
    LoadedExecutable executable = load_executable_into_memory(elf);

    // FIXME: Do this properly
    Kernel::Process::current();

    dbgln("Switching to shell process");

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

void initialize_uart_debug()
{
    // FIXME: I really don't know that the SDK does exactly, but this is necessary to make assertions work.
    stdio_init_all();

    // FIXME: For some reason there is a 0xff symbol send when the connection is opened.
    char ch = uart_getc(uart0);
    VERIFY(ch == 0xff);
}

int main()
{
    initialize_uart_debug();
    dbgln("\e[0;1mBOOT\e[0m");

    Kernel::MemoryAllocator::the();
    Kernel::MemoryFileSystem::the();
    Kernel::FlashFileSystem::the();
    Kernel::DeviceFileSystem::the();

    // FIXME: This is really ugly, not sure how to fix it
    dbgln("[main] Creating /example.txt");
    auto& example_file = Kernel::MemoryFileSystem::the().create_regular();
    auto& example_dentry = dynamic_cast<Kernel::MemoryDirectoryEntry&>(Kernel::MemoryFileSystem::the().create_directory_entry());
    example_dentry.m_file = dynamic_cast<Kernel::MemoryFile*>(&example_file);
    auto& example_handle = example_file.create_handle();
    example_handle.write({ (const u8*)"Hello, world!\n", 14 });
    Kernel::FileSystem::lookup("/").m_entries.append("example.txt", &example_dentry);

    load_and_execute_shell();

    for(;;)
        __wfi();
}
