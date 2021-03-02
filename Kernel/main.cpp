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
    auto *header = reinterpret_cast<Elf32_Ehdr*>(_binary_Shell_elf_start);
    auto *sections = reinterpret_cast<Elf32_Shdr*>(_binary_Shell_elf_start + header->e_shoff);

    char *strings = reinterpret_cast<char*>(_binary_Shell_elf_start + sections[header->e_shstrndx].sh_offset);

    // We can do this because we only have one segment which has virtual and physical address zero.
    void (*entry)() = reinterpret_cast<void(*)()>(_binary_Shell_elf_start + header->e_entry);

    for (usize index = 0; index < header->e_shnum; ++index)
        printf("Got section '%s'.\n", strings + sections[index].sh_name);
}

int main() {
    stdio_init_all();

    printf("\033[1mBOOT\033[0m\n");

    load_and_execute_shell();

    for(;;)
        __wfi();
}
