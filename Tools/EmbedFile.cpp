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

class ELF {
public:
    ELF()
    {
        int fd = memfd_create("memfd:elf", 0);
        assert(fd >= 0);

        m_data = fdopen(fd, "w");
        assert(m_data != nullptr);

        int retval = fseek(m_data, sizeof(Elf32_Ehdr), SEEK_CUR);
        assert(retval == 0);

        Elf32_Shdr shdr;
        shdr.sh_type = SHT_NULL;
        shdr.sh_name = append_section_name("");
        m_sections.push_back(shdr);
    }
    ~ELF()
    {
        int retval = fclose(m_data);
        assert(retval == 0);
    }

    Elf32_Shdr& append_section(std::string_view name, std::span<const uint8_t> data)
    {
        Elf32_Shdr shdr;
        shdr.sh_addr = ftell(m_data) - sizeof(Elf32_Ehdr);
        shdr.sh_addralign = 4;
        shdr.sh_entsize = 0;
        shdr.sh_flags = SHF_ALLOC;
        shdr.sh_info = 0;
        shdr.sh_link = 0;
        shdr.sh_name = append_section_name(name);
        shdr.sh_offset = ftell(m_data);
        shdr.sh_size = data.size();
        shdr.sh_type = SHT_PROGBITS;
        
        write_bytes(data);

        return m_sections.emplace_back(shdr);
    }

    FILE* finalize()
    {
        assert(!m_finalized);
        m_finalized = true;

        size_t section_offset;
        size_t shstrtab_section_index;
        encode_sections(section_offset, shstrtab_section_index);

        encode_header(section_offset, shstrtab_section_index);

        fseek(m_data, 0, SEEK_SET);
        return m_data;
    }

private:
    size_t append_section_name(std::string_view name)
    {
        size_t offset = m_shstrtab.tellp();
        m_shstrtab << name;
        m_shstrtab << '\0';
        return offset;
    }

    template<typename T>
    void write(const T& value)
    {
        write_bytes({ (const uint8_t*)&value, sizeof(value) });
    }

    void write_bytes(std::span<const uint8_t> data)
    {
        size_t retval = fwrite(data.data(), 1, data.size_bytes(), m_data);
        assert(retval == data.size_bytes());
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
        shdr.sh_offset = ftell(m_data);
        shdr.sh_addr = ftell(m_data) - sizeof(Elf32_Ehdr);
        shdr.sh_size = shstrtab.size();
        write_bytes({ (const uint8_t*)shstrtab.data(), shstrtab.size() });

        shstrtab_section_index = m_sections.size();
        m_sections.push_back(shdr);
    }

    void encode_sections(size_t& section_offset, size_t& shstrtab_section_index)
    {
        append_shstrtab_section(shstrtab_section_index);

        section_offset = ftell(m_data);

        for (const Elf32_Shdr& section : m_sections)
            write(section);
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

        fseek(m_data, 0, SEEK_SET);
        write(ehdr);
    }

    bool m_finalized = false;
    FILE *m_data;
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

    FILE *fp = elf.finalize();

    struct stat statbuf;
    fstat(fileno(fp), &statbuf);

    FILE *output = fopen("Shell.embedded.elf", "w");
    ssize_t retval = copy_file_range(fileno(fp), nullptr, fileno(output), nullptr, statbuf.st_size, 0);
    assert(retval == statbuf.st_size);
    fclose(output);
}
