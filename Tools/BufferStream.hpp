#pragma once

#include <cassert>
#include <utility>
#include <span>

#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>

inline std::span<const uint8_t> mmap_file(int fd)
{
    struct stat statbuf;

    int retval = fstat(fd, &statbuf);
    assert(retval == 0);

    void *pointer = mmap(nullptr, statbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    assert(pointer != MAP_FAILED);

    return { (const uint8_t*)pointer, (size_t)statbuf.st_size };
}

inline std::span<const uint8_t> mmap_file(std::filesystem::path path)
{
    int fd = open(path.c_str(), O_RDONLY);
    assert(fd >= 0);

    return mmap_file(fd);
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
        size_t offset = this->offset();

        size_t retval = fwrite(bytes.data(), 1, bytes.size(), m_file);
        assert(retval == bytes.size_bytes());

        return offset;
    }

    template<typename T>
    size_t write_object(const T& value)
    {
        return write_bytes({ (const uint8_t*)&value, sizeof(value) });
    }

    size_t offset()
    {
        long offset = ftell(m_file);
        assert(offset >= 0);
        return (size_t)offset;
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

    void copy_to(int fd)
    {
        ssize_t retval;

        retval = fseek(m_file, 0, SEEK_END);
        assert(retval == 0);

        size_t size = offset();

        retval = fseek(m_file, 0, SEEK_SET);
        assert(retval == 0);

        retval = copy_file_range(fileno(m_file), nullptr, fd, nullptr, size, 0);
        assert(retval == size);
    }

    std::span<const uint8_t> map_into_memory()
    {
        seek(0);
        return mmap_file(fileno(m_file));
    }

private:
    FILE *m_file;
};
