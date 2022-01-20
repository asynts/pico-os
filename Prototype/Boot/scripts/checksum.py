#!/usr/bin/env python3

# Copies the input file and computes the checksum of the '.boot_2_flash_second_stage' section.
# The checksum is placed in the last four bytes of the output file.

import sys
import zlib
import struct

import elftools.elf.elffile

def main(input_filename, output_filename):
    with open(output_filename, "wb") as output_file:
        with open(input_filename, "rb") as input_file:
            # First, we copy the input file into the output file.
            output_file.write(input_file.read())

            input_file.seek(0)
            output_file.seek(0)

            elf_file = elftools.elf.elffile.ELFFile(input_file)

            target_section = elf_file.get_section_by_name(".boot_2_flash_second_stage")
            assert target_section.data_size == 256

            # We read the contents of the boot section into a buffer.
            data = bytearray(target_section.data())

            # We insert the checksum into the buffer.
            checksum = zlib.crc32(data, 0xffffffff)
            data[-4:] = struct.pack("<I", checksum)

            offset = target_section["sh_offset"]

            # We write the buffer into the output file.
            output_file.seek(offset)
            output_file.write(data)

if __name__ == "__main__":
    assert len(sys.argv) == 3
    main(sys.argv[1], sys.argv[2])
