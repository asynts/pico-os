#include <fcntl.h>
#include <assert.h>
#include <unistd.h>

#include <LibElf/Generator.hpp>
#include <LibElf/MemoryStream.hpp>
#include <LibElf/SymbolTable.hpp>

static void write_output_file(std::filesystem::path path, Elf::MemoryStream& stream)
{
    int fd = creat(path.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    assert(fd >= 0);

    stream.copy_to_raw_fd(fd);

    int retval = close(fd);
    assert(retval == 0);
}

int main()
{
    Elf::Generator generator;

    Elf::MemoryStream data_stream;
    data_stream.write_object<uint32_t>(0);
    data_stream.write_object<uint32_t>(32); // target
    data_stream.write_object<uint32_t>(0);
    size_t data_index = generator.append_section(".data", data_stream, SHT_PROGBITS, SHF_ALLOC);

    Elf::SymbolTable symbol_table { "data", data_index };
    size_t data_symbol = symbol_table.add_symbol(".data", Elf32_Sym {
        .st_value = 0,
        .st_size = 12,
        .st_info = ELF32_ST_INFO(STB_LOCAL, STT_SECTION),
        .st_other = STV_DEFAULT,
    });
    symbol_table.add_relocation(Elf32_Rel {
       .r_offset = 4,
       .r_info = ELF32_R_INFO(data_symbol, R_ARM_ABS32),
    });
    symbol_table.apply(generator);

    auto stream = std::move(generator).finalize();
    write_output_file("Output.elf", stream);
}
