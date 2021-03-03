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
    auto *segments = reinterpret_cast<Elf32_Phdr*>(_binary_Shell_elf_start + header->e_phoff);
    auto *sections = reinterpret_cast<Elf32_Shdr*>(_binary_Shell_elf_start + header->e_shoff);
    char *strings = reinterpret_cast<char*>(_binary_Shell_elf_start + sections[header->e_shstrndx].sh_offset);

    // Since this processor doesn't have an MMU we are only using two sections. One for readonly sections
    // and another for writable sections.
    //
    // The readonly sections will be referenced relative to PC and the writable stuff will be referenced
    // relative to SB, thus we are executing position independent.
    assert(header->e_phnum == 2);

    // Load segments into memory.
    u8 *heap_segment = nullptr;
    for (usize index = 0; index < header->e_phnum; ++index) {
        assert(segments[index].p_type == PT_LOAD);

        if ((segments[index].p_flags & PF_W) == 0 && segments[index].p_filesz == segments[index].p_memsz) {
            printf("Loaded segment %zu in FLASH at %p.\n", index, _binary_Shell_elf_start + segments[index].p_offset);
            continue;
        }

        assert(heap_segment == nullptr);
        heap_segment = new u8[segments[index].p_memsz];
        __builtin_memcpy(heap_segment, _binary_Shell_elf_start + segments[index].p_offset, segments[index].p_memsz);

        printf("Loaded segment %zu in RAM at %p.\n", index, heap_segment);
    }
    assert(heap_segment != nullptr);

    u8 *stack = new u8[0x2000] + 0x2000;
    assert(u32(stack) % 8 == 0);
    printf("Allocated stack in RAM at %p.\n", stack);

    // We can compute the entry point address like this because the readonly segment starts at V0x00000000.
    auto *entry = reinterpret_cast<void(*)()>(_binary_Shell_elf_start + segments[0].p_offset + header->e_entry);

    // FIXME: Setup PIC register. (SB)

    // Switch to process stack pointer and execute unprivileged.
    asm volatile(
        "msr psp, %0;"
        "isb;"
        "movs r0, #0b11;"
        "msr control, r0;"
        "isb;"
        "blx %1;"
        :
        : "r"(stack), "r"(entry));

    panic("Process returned, it should not.\n");
}

int main() {
    stdio_init_all();

    printf("\033[1mBOOT\033[0m\n");

    load_and_execute_shell();

    for(;;)
        __wfi();
}
