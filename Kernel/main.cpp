#include <pico/stdio.h>
#include <pico/printf.h>
#include <pico/sync.h>

#include <Std/Forward.hpp>

#include <elf.h>

extern "C" {
    extern u8 _binary_Shell_elf_start[];
}

void load_and_execute_shell()
{
    auto *elf_header = reinterpret_cast<Elf32_Ehdr*>(_binary_Shell_elf_start);
    auto *program_header = reinterpret_cast<Elf32_Phdr*>(_binary_Shell_elf_start + elf_header->e_phoff);
    auto *section_header = reinterpret_cast<Elf32_Shdr*>(_binary_Shell_elf_start + elf_header->e_shoff);
}

int main() {
    stdio_init_all();

    printf("\033[1mBOOT\033[0m\n");

    load_and_execute_shell();

    for(;;)
        __wfi();
}
