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
    
    FileSystemGenerator fs_generator { elf_generator };
    fs_generator.create_file("/bin/Shell.elf", mmap_file("Userland/Shell.elf"));
    std::move(fs_generator).finalize();

    BufferStream stream = std::move(elf_generator).finalize();

    int output_fd = creat("Shell.embed.elf", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    assert(output_fd >= 0);

    stream.copy_to(output_fd);

    int retval = close(output_fd);
    assert(retval == 0);
}
