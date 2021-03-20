#pragma once

#include <span>
#include <filesystem>

#include <stdio.h>

namespace Elf
{
    std::span<const uint8_t> mmap_file(std::filesystem::path);

    class MemoryStream {
    public:
        MemoryStream();
        ~MemoryStream();
        MemoryStream(MemoryStream&&);

        size_t write_bytes(std::span<const uint8_t>);
        size_t write_bytes(MemoryStream&);

        template<typename T>
        size_t write_object(const T& value)
        {
            return write_bytes({ (const uint8_t*)&value, sizeof(value) });
        }

        size_t offset();
        size_t size();

        void seek(size_t);
        void seek_relative(ssize_t);

        void copy_to_raw_fd(int fd);

    private:
        FILE *m_file;
    };
}
