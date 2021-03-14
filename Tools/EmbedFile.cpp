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

int main() {
    ElfGenerator elf_generator;

    // FIXME: Move this into FileSystemGenerator.
    Elf32_Shdr binary_shdr = elf_generator.append_section(".embedded.binary", mmap_file("Userland/Shell.elf"));

    FileSystemGenerator fs_generator;
    fs_generator.add_file("/bin/Shell.elf", binary_shdr.sh_addr, binary_shdr.sh_size);

    std::move(fs_generator).finalize(elf_generator);

    BufferStream binary = std::move(elf_generator).finalize();

    int output_fd = creat("Shell.embedded.elf", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    assert(output_fd >= 0);

    binary.copy_to(output_fd);

    int retval = close(output_fd);
    assert(retval == 0);
}
