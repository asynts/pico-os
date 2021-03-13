#!/usr/bin/env python3

import dataclasses
import enum
import io

# FIXME: I need to write this in C++ otherwise I'll have to redefine the whole elf header.

class ElfType(enum.Enum):
    ET_REL = 1

@dataclasses.dataclass
class ElfHeader:
    SIZE = 13 * 4

    e_ident: bytes = b"\x7fELF"
    e_type: int
    e_machine: int
    e_version: int
    e_entry: int
    e_phoff: int
    e_shoff: int
    e_flags: int
    e_ehsize: int
    e_phentsize: int
    e_phnum: int
    e_shentsize: int
    e_shnum: int
    e_shstrndx: int

class SectionType(enum.Enum):
    SHT_PROGBITS = 1

class SectionFlags(enum.Enum):
    SHF_ALLOC = 1 << 1

@dataclasses.dataclass
class SectionHeader:
    sh_name: str
    sh_type: SectionType
    sh_flags: SectionFlags
    sh_addr: int
    sh_offset: int
    sh_size: int
    sh_link: int
    sh_info: int
    sh_addralign: int
    sh_entsize: int

class ELF:
    def __init__(self):
        self._sections = []
        self._data = io.BytesIO()

        self._data.seek(ElfHeader.SIZE)

    def append_section(self, name: str, data: bytes):
        section = SectionHeader(
            sh_name=name,
            sh_type=SectionType.SHT_PROGBITS,
            sh_flags=SectionFlags.SHF_ALLOC)

        section.sh_offset = self._data.tell()
        self._data.write(data)

        self._sections.append(section)

    def finalize(self):
        elf = ElfHeader(
            e_type=)

elf = ELF()

with open("Userland/Shell.elf", "rb") as fp:
    elf.append_section(".embedded.shell", fp.read())
