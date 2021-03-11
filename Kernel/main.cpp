#include <pico/sync.h>
#include <hardware/uart.h>
#include <hardware/gpio.h>

#include <Std/Forward.hpp>
#include <Std/Debug.hpp>
#include <Kernel/DynamicLoader.hpp>

void load_and_execute_shell()
{
    ElfWrapper elf { reinterpret_cast<u8*>(embedded_shell_binary_start) };
    LoadedExecutable executable = load_executable_into_memory(elf);

    dbgprintf("Loading process stack and static base, debugger hook\n");

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
    uart_init(uart0, 115200);
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);

    // FIXME: For some reason there is a 0xff symbol send when the connection is opened.
    char ch = uart_getc(uart0);
    assert(ch == 0xff);
}

int main()
{
    initialize_uart_debug();
    dbgprintf("\033[1mBOOT\033[0m\n");

    load_and_execute_shell();

    for(;;)
        __wfi();
}
