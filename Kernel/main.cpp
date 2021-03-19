#include <pico/sync.h>
#include <hardware/uart.h>
#include <hardware/gpio.h>

#include <Std/Forward.hpp>
#include <Std/Format.hpp>
#include <Kernel/DynamicLoader.hpp>
#include <Kernel/ConsoleDevice.hpp>
#include <Kernel/FileSystem.hpp>

#include <pico/stdio.h>

extern "C" {
    extern u8 __fs_start[];
    extern u8 __fs_end[];
}

void load_and_execute_shell()
{
    auto& shell_dentry_info = Kernel::VirtualFileSystem::the().lookup_path("/bin/Shell.elf");

    dbgln("Found Shell.elf: inode=% size=%", shell_dentry_info.m_info->m_id, shell_dentry_info.m_info->m_size);

    ElfWrapper elf { shell_dentry_info.m_info->m_direct_blocks[0] };
    LoadedExecutable executable = load_executable_into_memory(elf);

    dbgln("Loading process stack and static base, debugger hook");

    asm volatile(
        "movs r0, #0;"
        "msr psp, r0;"
        "isb;"
        "movs r0, #0b11;"
        "msr control, r0;"
        "isb;"
        "mov r0, %1;"
        "mov sb, %2;"
        "bkpt #0;"
        "blx %0;"
        :
        : "r"(executable.m_entry), "r"(executable.m_stack_base + executable.m_stack_size), "r"(executable.m_writable_base)
        : "r0");

    panic("Process returned, it shouldn't have\n");
}

void initialize_uart_debug()
{
    // FIXME: I really don't know that the SDK does exactly, but this is necessary to make assertions work.
    stdio_init_all();

    // FIXME: For some reason there is a 0xff symbol send when the connection is opened.
    char ch = uart_getc(uart0);
    assert(ch == 0xff);
}

int main()
{
    initialize_uart_debug();
    dbgln("\e[1mBOOT\e[0m");

    Kernel::VirtualFileSystem::the();
    Kernel::ConsoleDevice::the();

    load_and_execute_shell();

    for(;;)
        __wfi();
}
