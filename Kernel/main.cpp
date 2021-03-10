#include <pico/stdio.h>
#include <pico/printf.h>
#include <pico/sync.h>

#include <Std/Forward.hpp>
#include <Kernel/DynamicLoader.hpp>

void load_and_execute_shell()
{
    ElfWrapper elf { reinterpret_cast<u8*>(embedded_shell_binary_start) };
    LoadedExecutable executable = load_executable_into_memory(elf);

    printf("Handing over execution to new process\n");

    // FIXME: Setup PIC register. (SB)

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

int main() {
    stdio_init_all();

    printf("\033[1mBOOT\033[0m\n");

    load_and_execute_shell();

    for(;;)
        __wfi();
}
