#include <fcntl.h>
#include <assert.h>
#include <unistd.h>

#include <LibElf/Generator.hpp>
#include <LibElf/MemoryStream.hpp>
#include <LibElf/SymbolTable.hpp>

#include "FileSystem.hpp"

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

    FileSystem fs { generator };

    fs.add_file("/bin/Shell.elf", Elf::mmap_file("../../Build/Userland/Shell.2.elf"));
    
    std::move(fs).finalize();

    auto stream = std::move(generator).finalize();
    write_output_file("Output.elf", stream);
}
