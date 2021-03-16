#include <pico/sync.h>
#include <hardware/uart.h>
#include <hardware/gpio.h>

#include <Std/Forward.hpp>
#include <Std/Format.hpp>
#include <Kernel/DynamicLoader.hpp>
#include <Kernel/ConsoleDevice.hpp>
#include <Kernel/MemoryFilesystem.hpp>

#include <pico/stdio.h>

// FIXME: This is replicated in Tools/FileSystem.cpp
struct IndexNode {
    uint32_t m_inode;
    uint32_t m_mode;
    uint32_t m_size;
    uint32_t m_device_id;
    uint32_t m_block_size;
    u8* m_direct_blocks[1];
    u8** m_indirect_blocks[4];
};
struct FlashEntry {
    char m_name[252];
    IndexNode *m_inode;
};
static_assert(sizeof(FlashEntry) == 256);

extern "C"
{
    extern char __embed_start[];
    extern char __embed_end[];
}

void load_and_execute_shell()
{
    assert(__embed_end - __embed_start == sizeof(FlashEntry));
    FlashEntry *shell_entry = reinterpret_cast<FlashEntry*>(__embed_start);

    dbgln("Loading Shell.elf from FlashEntry { m_name=%, m_inode=% } from address %",
        shell_entry->m_name,
        shell_entry->m_inode,
        shell_entry->m_inode->m_direct_blocks[0]);

    ElfWrapper elf { reinterpret_cast<u8*>(shell_entry->m_inode->m_direct_blocks[0]) };
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

    load_and_execute_shell();

    for(;;)
        __wfi();
}
