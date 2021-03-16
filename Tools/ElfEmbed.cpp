#include <map>

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
    std::optional<std::string_view> output_file;
    std::map<std::string_view, std::string_view> input_files;
    for (int index = 1; index < argc; ++index) {
        assert(index + 1 < argc);

        if (std::string_view { "-o" } == argv[index]) {
            assert(!output_file.has_value());
            output_file = argv[index + 1];
            ++index;
        } else {
            input_files[argv[index]] = argv[index+1];
            ++index;
        }
    }
    assert(output_file.has_value());

    Elf::Generator generator;

    FileSystem fs { generator };

    for (auto& [real_path, virtual_path] : input_files) {
        fmt::print("Adding input file {} ({})\n", virtual_path, real_path);
        fs.add_file(virtual_path , Elf::mmap_file(real_path));
    }

    fs.finalize();

    auto stream = generator.finalize();
    write_output_file(output_file.value(), stream);
}
