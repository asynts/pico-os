#include <pico/stdio.h>
#include <pico/printf.h>
#include <pico/sync.h>

#include <Std/Forward.hpp>

#include <elf.h>

extern "C" {
    extern u8 _binary_Shell_elf_start[];
    extern u8 _binary_Shell_elf_end[];
    extern u8 _binary_Shell_elf_size[];
}

class ElfWrapper {
public:
    explicit ElfWrapper(u8 *base)
        : m_base(base)
    {
    }

    u8* base() { return m_base; }
    u32 base_as_u32() { return reinterpret_cast<u32>(m_base); }

    usize offset() { return m_offset; }

    Elf32_Ehdr* header() { return reinterpret_cast<Elf32_Ehdr*>(m_base); }
    Elf32_Phdr* segments() { return reinterpret_cast<Elf32_Phdr*>(m_base + header()->e_phoff); }
    Elf32_Shdr* sections() { return reinterpret_cast<Elf32_Shdr*>(m_base + header()->e_shoff); }

    char* section_name_base() { return reinterpret_cast<char*>(m_base + sections()[header()->e_shstrndx].sh_offset); }

    void append(void *data, usize size)
    {
        __builtin_memcpy(m_base + m_offset, data, size);
        m_offset += size;
    }

    void consume(usize size)
    {
        m_offset += size;
    }

private:
    u8 *m_base;
    usize m_offset = 0;
};

usize compute_size_in_memory(ElfWrapper elf)
{
    usize size = 0;

    size += sizeof(Elf32_Ehdr);

    for (usize index = 0; index < elf.header()->e_phnum; ++index)
        size += sizeof(Elf32_Phdr) + elf.segments()[index].p_memsz;

    for (usize index = 0; index < elf.header()->e_shnum; ++index) {
        auto& section = elf.sections()[index];

        size += sizeof(Elf32_Shdr);

        // These sections are not placed in segments, thus they are not accounted for.
        if ((section.sh_flags & SHF_ALLOC) == 0)
            size += section.sh_size;
    }

    printf("Computed memory size of binary %p as %zu\n", elf.base(), size);
    return size;
}

void patch_executable(ElfWrapper flash_elf, ElfWrapper ram_elf)
{
    printf("Constructing binary %p from binary %p\n", ram_elf.base(), flash_elf.base());

    // Copy headers over.
    ram_elf.append(flash_elf.header(), sizeof(Elf32_Ehdr));

    ram_elf.header()->e_phoff = ram_elf.offset();
    ram_elf.append(flash_elf.segments(), sizeof(Elf32_Phdr) * flash_elf.header()->e_phnum);

    ram_elf.header()->e_shoff = ram_elf.offset();
    ram_elf.append(flash_elf.sections(), sizeof(Elf32_Shdr) * flash_elf.header()->e_shnum);

    // Patch segments.
    for (usize index = 0; index < ram_elf.header()->e_phnum; ++index) {
        auto &ram_segment = ram_elf.segments()[index];
        auto &flash_segment = flash_elf.segments()[index];

        if (flash_segment.p_type == PT_NULL) {
            printf("Skipping segment %zu\n", index);
            continue;
        }

        ram_segment.p_vaddr = ram_elf.base_as_u32() + ram_elf.offset();
        ram_segment.p_offset = ram_elf.offset();

        printf("Patched segment %zu with vaddr=%p offset=%zu\n", index, ram_segment.p_vaddr, ram_segment.p_offset);

        ram_elf.append(flash_elf.base() + flash_segment.p_offset, flash_segment.p_filesz);

        assert(flash_segment.p_memsz >= flash_segment.p_filesz);
        ram_elf.consume(flash_segment.p_memsz - flash_segment.p_filesz);

        // Patch entry point if it falls into this segment.
        if (flash_elf.header()->e_entry >= flash_segment.p_vaddr &&
            flash_elf.header()->e_entry <= flash_segment.p_vaddr + flash_segment.p_memsz)
        {
            ram_elf.header()->e_entry = ram_segment.p_vaddr + (flash_elf.header()->e_entry - flash_segment.p_vaddr);
            printf("Patched entry point in segment %zu from %p to %p\n", index, flash_elf.header()->e_entry, ram_elf.header()->e_entry);
        }
    }

    // Patch sections.
    for (usize section_index = 0; section_index < ram_elf.header()->e_shnum; ++section_index) {
        auto &ram_section = ram_elf.sections()[section_index];
        auto &flash_section = flash_elf.sections()[section_index];

        const char *flash_section_name = flash_elf.section_name_base() + flash_section.sh_name;

        if (flash_section.sh_type == SHT_NULL) {
            printf("Skipping section '%s'\n", flash_section_name);
            continue;
        }

        if (flash_section.sh_flags & SHF_ALLOC) {
            printf("Searching for section '%s'\n", flash_section_name);

            i32 offset_from_segment = -1;
            usize segment_index;
            for (segment_index = 0; segment_index < flash_elf.header()->e_phnum; ++segment_index) {
                auto &flash_segment = flash_elf.segments()[segment_index];

                if (flash_segment.p_type == PT_NULL)
                    continue;

                if (flash_section.sh_offset >= flash_segment.p_offset &&
                    flash_section.sh_offset <= flash_segment.p_offset + flash_segment.p_filesz)
                {
                    offset_from_segment = flash_section.sh_offset - flash_segment.p_offset;
                    break;
                }
            }
            assert(offset_from_segment >= 0);

            printf("Found section '%s' in segment %zu with offset %zu\n",
                flash_section_name,
                segment_index,
                offset_from_segment);

            auto &ram_segment = ram_elf.segments()[segment_index];
            auto &flash_segment = flash_elf.segments()[segment_index];

            ram_section.sh_addr = ram_segment.p_vaddr + offset_from_segment;
            ram_section.sh_offset = ram_segment.p_offset + offset_from_segment;

            printf("Patched section '%s' with addr=%p offset=%zu\n",
                flash_section_name,
                ram_section.sh_addr,
                ram_section.sh_offset);
        } else {
            printf("Section '%s' is non-allocating, loading seperately.\n", flash_section_name);
            assert((flash_section.sh_flags & SHF_ALLOC) == 0);

            ram_section.sh_offset = ram_elf.offset();
            ram_section.sh_addr = ram_elf.base_as_u32() + ram_elf.offset();

            ram_elf.append(flash_elf.base() + flash_section.sh_offset, flash_section.sh_size);
        }
    }
}

void load_and_execute_shell()
{
    ElfWrapper flash_elf { reinterpret_cast<u8*>(_binary_Shell_elf_start) };
    ElfWrapper ram_elf { new u8[compute_size_in_memory(flash_elf)] };

    patch_executable(flash_elf, ram_elf);

    u8 *stack = new u8[0x2000] + 0x2000;
    assert(u32(stack) % 8 == 0);
    printf("Allocated stack in RAM at %p\n", stack);

    // FIXME: What about the heap?

    printf("Finished loading executable\n");

    // FIXME: Inform the debugger about the loaded executable.

    // FIXME: Setup PIC register. (SB)

    __breakpoint();

    printf("Handing over execution to new process\n");

    // Switch to process stack pointer and execute unprivileged.
    asm volatile(
        "msr psp, %0;"
        "isb;"
        "movs r0, #0b11;"
        "msr control, r0;"
        "isb;"
        "blx %1;"
        :
        : "r"(stack), "r"(ram_elf.header()->e_entry));

    panic("Process returned, it shouldn't have\n");
}

int main() {
    stdio_init_all();

    printf("\033[1mBOOT\033[0m\n");

    load_and_execute_shell();

    for(;;)
        __wfi();
}
