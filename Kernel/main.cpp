#include <pico/sync.h>
#include <hardware/uart.h>
#include <hardware/gpio.h>

#include <Std/Forward.hpp>
#include <Std/Format.hpp>
#include <Kernel/DynamicLoader.hpp>
#include <Kernel/ConsoleDevice.hpp>
#include <Kernel/MemoryFilesystem.hpp>

#include <pico/stdio.h>

void load_and_execute_shell()
{
    auto& shell_file = Kernel::iterate_path("/bin/Shell.elf");

    dbgln("Found Shell.elf: inode=% size=%", shell_file.m_inode, shell_file.m_size);

    ElfWrapper elf { shell_file.m_direct_blocks[0] };
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

    Kernel::MemoryFilesystem::the();
    Kernel::ConsoleDevice::the();

    dbgln("The contents of '/':");
    Kernel::iterate_directory(Kernel::MemoryFilesystem::the().root(), [](auto entry) {
        dbgln("  % (%)", entry.m_name, entry.m_inode);
        return Kernel::IterationDecision::Continue;
    });

    load_and_execute_shell();

    for(;;)
        __wfi();
}
