#!/usr/bin/env python3

# Copies the input file and computes the checksum of the '.boot_2_flash_second_stage' section.
# The checksum is placed in the last four bytes of the output file.

import sys
import zlib
import struct

import elftools.elf.elffile

def main(input_filename, output_filename):
    with open(output_filename, "wb+") as output_file:
        with open(input_filename, "rb") as input_file:
            output_file.write(input_file.read())

            input_file.seek(0)
            output_file.seek(0)

            elf_file = elftools.elf.elffile.ELFFile(input_file)

            target_section = elf_file.get_section_by_name(".boot_2_flash_second_stage")
            assert target_section.data_size == 256

            data = bytearray(target_section.data())

            checksum = zlib.crc32(data)
            data[-4:] = struct.pack("<I", checksum)

            header_size = target_section.structs.Elf_Chdr.sizeof()
            offset = target_section["sh_offset"] + header_size

            output_file.seek(offset)

            # FIXME: I don't think we replace here.
            output_file.write(data)

if __name__ == "__main__":
    assert len(sys.argv) == 3
    main(sys.argv[1], sys.argv[2])
