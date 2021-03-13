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
#include <assert.h>

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
        fclose(m_file);
    }

    void write_bytes(std::span<const uint8_t> bytes)
    {
        size_t retval = fwrite(bytes.data(), 1, bytes.size(), m_file);
        assert(retval == bytes.size_bytes());
    }

    template<typename T>
    void write_object(const T& value)
    {
        write_bytes({ (const uint8_t*)&value, sizeof(value) });
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

private:
    FILE *m_file;
};

class ELF {
public:
    ELF()
    {
        m_stream.seek(sizeof(Elf32_Ehdr));

        Elf32_Shdr shdr;
        shdr.sh_type = SHT_NULL;
        shdr.sh_name = append_section_name("");
        m_sections.push_back(shdr);
    }

    Elf32_Shdr& append_section(std::string_view name, std::span<const uint8_t> data)
    {
        Elf32_Shdr shdr;
        shdr.sh_addr = m_stream.offset() - sizeof(Elf32_Ehdr);
        shdr.sh_addralign = 4;
        shdr.sh_entsize = 0;
        shdr.sh_flags = SHF_ALLOC;
        shdr.sh_info = 0;
        shdr.sh_link = 0;
        shdr.sh_name = append_section_name(name);
        shdr.sh_offset = m_stream.offset();
        shdr.sh_size = data.size();
        shdr.sh_type = SHT_PROGBITS;
        
        m_stream.write_bytes(data);

        return m_sections.emplace_back(shdr);
    }

    BufferStream& finalize()
    {
        assert(!m_finalized);
        m_finalized = true;

        size_t section_offset;
        size_t shstrtab_section_index;
        encode_sections(section_offset, shstrtab_section_index);

        encode_header(section_offset, shstrtab_section_index);

        m_stream.seek(0);
        return m_stream;
    }

private:
    size_t append_section_name(std::string_view name)
    {
        size_t offset = m_shstrtab.tellp();
        m_shstrtab << name;
        m_shstrtab << '\0';
        return offset;
    }

    void append_shstrtab_section(size_t& shstrtab_section_index)
    {
        Elf32_Shdr shdr;
        shdr.sh_addralign = 4;
        shdr.sh_entsize = 0;
        shdr.sh_flags = 0;
        shdr.sh_info = 0;
        shdr.sh_link = 0;
        shdr.sh_name = append_section_name(".shstrtab");
        shdr.sh_type = SHT_STRTAB;

        std::string shstrtab = m_shstrtab.str();
        shdr.sh_offset = m_stream.offset();
        shdr.sh_addr = m_stream.offset() - sizeof(Elf32_Ehdr);
        shdr.sh_size = shstrtab.size();
        m_stream.write_bytes({ (const uint8_t*)shstrtab.data(), shstrtab.size() });

        shstrtab_section_index = m_sections.size();
        m_sections.push_back(shdr);
    }

    void encode_sections(size_t& section_offset, size_t& shstrtab_section_index)
    {
        append_shstrtab_section(shstrtab_section_index);

        section_offset = m_stream.offset();

        for (const Elf32_Shdr& section : m_sections)
            m_stream.write_object(section);
    }

    void encode_header(size_t section_offset, size_t shstrtab_section_index)
    {
        Elf32_Ehdr ehdr;
        ehdr.e_ehsize = sizeof(Elf32_Ehdr);
        ehdr.e_entry = 0;
        ehdr.e_flags = 0x05000000;
        
        memcpy(ehdr.e_ident, ELFMAG, SELFMAG);
        ehdr.e_ident[EI_CLASS] = ELFCLASS32;
        ehdr.e_ident[EI_DATA] = ELFDATA2LSB;
        ehdr.e_ident[EI_VERSION] = EV_CURRENT;
        ehdr.e_ident[EI_OSABI] = ELFOSABI_NONE;
        ehdr.e_ident[EI_ABIVERSION] = 0;

        ehdr.e_machine = EM_ARM;
        ehdr.e_phentsize = sizeof(Elf32_Phdr);
        ehdr.e_phnum = 0;
        ehdr.e_phoff = 0;
        ehdr.e_shentsize = sizeof(Elf32_Shdr);
        ehdr.e_shnum = m_sections.size();
        ehdr.e_shoff = section_offset;
        ehdr.e_shstrndx = shstrtab_section_index;
        ehdr.e_type = ET_REL;
        ehdr.e_version = EV_CURRENT;

        m_stream.seek(0);
        m_stream.write_object(ehdr);
    }

    BufferStream m_stream;

    bool m_finalized = false;
    std::vector<Elf32_Shdr> m_sections;
    std::ostringstream m_shstrtab;
};

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

int main() {
    ELF elf;
    elf.append_section(".embedded.binary", mmap_file("Userland/Shell.elf"));

    BufferStream& elf_binary = elf.finalize();

    int output_fd = open("Shell.embedded.elf", O_WRONLY);
    assert(output_fd >= 0);

    elf_binary.copy_to(output_fd);

    int retval = close(output_fd);
    assert(retval == 0);
}
