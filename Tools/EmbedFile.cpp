#include "BufferStream.hpp"
#include "ElfGenerator.hpp"
#include "FileSystemGenerator.hpp"

#include <cstdio>
#include <string_view>
#include <span>
#include <filesystem>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <elf.h>
#include <unistd.h>
#include <bsd/string.h>
#include <assert.h>

void write_output_file(std::filesystem::path path, BufferStream& stream)
{
    int fd = creat(path.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    assert(fd >= 0);

    stream.copy_to_raw_fd(fd);

    int retval = close(fd);
    assert(retval == 0);
}

int main()
{
    // FIXME: I think the problem is that this relocation is marked to break unless compiled with -fpic and
    //        that is somehow kept in the relocatable?
    // 
    // $ arm-none-eabi-ld Output.elf -o Linked.elf
    // arm-none-eabi-ld: warning: cannot find entry symbol _start; defaulting to 0000000000008000
    // Output.elf:(.data+0x4): dangerous relocation: unsupported relocation

    ElfGenerator elf_generator;

    size_t data_section_index;
    {
        BufferStream data_stream;
        data_stream.write_object<uint32_t>(0);
        data_stream.write_object<uint32_t>(32); // target
        data_stream.write_object<uint32_t>(0);

        data_section_index = elf_generator.append_section(".data", data_stream);
    }
    auto& data_section = elf_generator.section(data_section_index);

    size_t strtab_section_index;
    {
        BufferStream strtab_stream;
        strtab_stream.write_object<uint8_t>(0);
        strtab_stream.write_bytes({ (const uint8_t*)"data\0", 5 });

        strtab_section_index = elf_generator.append_section(".strtab", strtab_stream, SHT_STRTAB, 0);
    }

    size_t symtab_section_index;
    {
        BufferStream symtab_stream;

        Elf32_Sym undefined_symbol;
        undefined_symbol.st_name = 0;
        undefined_symbol.st_value = 0;
        undefined_symbol.st_size = 0;
        undefined_symbol.st_info = 0;
        undefined_symbol.st_other = 0;
        undefined_symbol.st_shndx = SHN_UNDEF;
        symtab_stream.write_object(undefined_symbol);

        Elf32_Sym section_symbol;
        section_symbol.st_name = 1;
        section_symbol.st_value = data_section.sh_addr;
        section_symbol.st_size = data_section.sh_size;
        section_symbol.st_info = ELF32_ST_INFO(STB_LOCAL, STT_SECTION);
        section_symbol.st_other = 0;
        section_symbol.st_shndx = data_section_index;
        symtab_stream.write_object(section_symbol);

        symtab_section_index = elf_generator.append_section(".symtab", symtab_stream, SHT_SYMTAB, 0);
        auto& symbol_section = elf_generator.section(symtab_section_index);

        symbol_section.sh_entsize = sizeof(Elf32_Sym);
        symbol_section.sh_link = strtab_section_index;
        symbol_section.sh_info = 2;
    }

    size_t rel_section_index;
    {
        BufferStream rel_stream;

        Elf32_Rel target_relocation;
        target_relocation.r_offset = 4; // target
        target_relocation.r_info = ELF32_R_INFO(1, R_ARM_ABS32);
        rel_stream.write_object(target_relocation);

        rel_section_index = elf_generator.append_section(".rel.data", rel_stream, SHT_REL, 0);
        auto& rel_section = elf_generator.section(rel_section_index);

        rel_section.sh_link = symtab_section_index;
        rel_section.sh_info = data_section_index;
        rel_section.sh_entsize = sizeof(Elf32_Rel);
    }

    auto stream = std::move(elf_generator).finalize();
    write_output_file("Output.elf", stream);
}
