#include <utility>

#include <fcntl.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

#include "MemoryStream.hpp"

namespace Elf
{
    std::span<const uint8_t> mmap_file(std::filesystem::path path)
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
    MemoryStream::MemoryStream()
    {
        int fd = memfd_create("MemoryStream", 0);
        assert(fd >= 0);

        m_file = fdopen(fd, "w");
        assert(m_file != nullptr);
    }
    MemoryStream::~MemoryStream()
    {
        if (m_file)
            fclose(m_file);
    }
    MemoryStream::MemoryStream(MemoryStream&& other)
    {
        m_file = std::exchange(other.m_file, nullptr);
    }
    size_t MemoryStream::write_bytes(std::span<const uint8_t> bytes)
    {
        size_t offset = this->offset();

        size_t retval = fwrite(bytes.data(), 1, bytes.size(), m_file);
        assert(retval == bytes.size_bytes());

        return offset;
    }
    size_t MemoryStream::write_bytes(MemoryStream& other)
    {
        size_t base_offset = this->offset();

        size_t offset = other.offset();

        other.seek(0);

        char buffer[0x1000];
        while (!feof(other.m_file)) {
            size_t nread = fread(buffer, 1, sizeof(buffer), other.m_file);
            size_t nwritten = fwrite(buffer, 1, nread, m_file);
            assert(nread == nwritten);
        }

        other.seek(offset);

        return base_offset;
    }
    size_t MemoryStream::offset()
    {
        long offset = ftell(m_file);
        assert(offset >= 0);
        return (size_t)offset;
    }
    size_t MemoryStream::size()
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
    void MemoryStream::seek(size_t offset)
    {
        int retval = fseek(m_file, offset, SEEK_SET);
        assert(retval == 0);
    }
    void MemoryStream::seek_relative(ssize_t offset)
    {
        int retval = fseek(m_file, offset, SEEK_CUR);
        assert(retval == 0);
    }
    void MemoryStream::copy_to_raw_fd(int fd)
    {
        off_t input_offset = 0;

        ssize_t retval = copy_file_range(fileno(m_file), &input_offset, fd, nullptr, size(), 0);
        assert(retval == size());
    }
}
