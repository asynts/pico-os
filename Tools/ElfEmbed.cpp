#include <map>
#include <vector>

#include <fcntl.h>
#include <assert.h>
#include <unistd.h>

#include <fmt/format.h>

#include <LibElf/Generator.hpp>
#include <LibElf/MemoryStream.hpp>
#include <LibElf/SymbolTable.hpp>

#include "FileSystem.hpp"

static void write_output_file(std::filesystem::path path, Elf::MemoryStream& stream)
{
    fmt::print("Writing output file {}\n", path.string());

    int fd = creat(path.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    assert(fd >= 0);

    stream.copy_to_raw_fd(fd);

    int retval = close(fd);
    assert(retval == 0);
}

int main(int argc, char **argv)
{
    // FIXME: Parse command line arguments

    Elf::Generator generator;

    FileSystem fs { generator };

    std::map<std::string, uint32_t> bin_files;
    bin_files["Shell.elf"] = fs.add_host_file("Shell.elf", Kernel::ModeFlags::Regular | Kernel::ModeFlags::DefaultExecutablePermissions);
    bin_files["Example.elf"] = fs.add_host_file("Example.elf", Kernel::ModeFlags::Regular | Kernel::ModeFlags::DefaultExecutablePermissions);

    fs.add_root_directory(bin_files);

    fs.finalize();

    auto stream = generator.finalize();
    write_output_file("FileSystem.elf", stream);
}
