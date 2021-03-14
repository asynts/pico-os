#pragma once

#include <cassert>
#include <utility>
#include <span>
#include <filesystem>

#include <sys/mman.h>
#include <sys/stat.h>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>

// FIXME
#include <iostream>

inline std::span<const uint8_t> mmap_file(std::filesystem::path path)
{
    int fd = open(path.c_str(), O_RDONLY);
    assert(fd >= 0);

    struct stat statbuf;

    int retval = fstat(fd, &statbuf);
    assert(retval == 0);

    void *pointer = mmap(nullptr, statbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    assert(pointer != MAP_FAILED);

    return { (const uint8_t*)pointer, (size_t)statbuf.st_size };
}

class BufferStream {
public:
    BufferStream()
    {
        int fd = memfd_create("BufferStream", 0);
        assert(fd >= 0);

        m_file = fdopen(fd, "w");
        assert(m_file != nullptr);
    }
    ~BufferStream()
    {
        if (m_file)
            fclose(m_file);
    }
    BufferStream(BufferStream&& other)
    {
        m_file = std::exchange(other.m_file, nullptr);
    }

    size_t write_bytes(std::span<const uint8_t> bytes)
    {
        printf("BufferStream::write_bytes size=%zu\n", bytes.size());

        size_t offset = this->offset();

        size_t retval = fwrite(bytes.data(), 1, bytes.size(), m_file);
        assert(retval == bytes.size_bytes());

        return offset;
    }

    size_t write_bytes(BufferStream& other)
    {
        printf("BufferStream::write_bytes size=%zu\n", other.size());

        size_t base_offset = this->offset();

        size_t offset = other.offset();

        other.seek(0);
        
        char buffer[0x1000];
        while (!feof(other.m_file)) {
            size_t nread = fread(buffer, 1, sizeof(buffer), other.m_file);
            size_t nwritten = fwrite(buffer, 1, nread, m_file);
            assert(nread == nwritten);

            printf("BufferStream::write_bytes forwarded %zu bytes\n", nread);

            if (nread == 36)
                std::cout << "\"" << std::string_view { buffer, nread } << "\"\n";
        }

        other.seek(offset);

        return base_offset;
    }

    template<typename T>
    size_t write_object(const T& value)
    {
        printf("BufferStream::write_object size=%zu\n", sizeof(value));

        return write_bytes({ (const uint8_t*)&value, sizeof(value) });
    }

    size_t offset()
    {
        long offset = ftell(m_file);
        assert(offset >= 0);
        return (size_t)offset;
    }

    size_t size()
    {
        int retval;

        size_t offset = this->offset();

        retval = fseek(m_file, 0, SEEK_END);
        assert(retval == 0);

        size_t size = this->offset();

        retval = fseek(m_file, offset, SEEK_SET);
        assert(retval == 0);

        return size;
    }

    void seek(size_t offset)
    {
        int retval = fseek(m_file, offset, SEEK_SET);
        assert(retval == 0);
    }

    void seek_relative(ssize_t offset)
    {
        int retval = fseek(m_file, offset, SEEK_CUR);
        assert(retval == 0);
    }

    void copy_to_raw_fd(int fd)
    {
        off_t input_offset = 0;

        ssize_t retval = copy_file_range(fileno(m_file), &input_offset, fd, nullptr, size(), 0);
        assert(retval == size());
    }

private:
    FILE *m_file;
};
