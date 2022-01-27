#!/usr/bin/env python3

# Copies the input file and computes the checksum of the '.boot_2_flash_second_stage' section.
# The checksum is patched into the output file.
# All debugging symbols are keept in the output file.

import sys
import struct
import binascii

import elftools.elf.elffile

def bitrev(value, bits):
    return int("{:0{width}b}".format(value, width=bits)[::-1], 2)

# It appears that there is some sort of micro-optimization in the bootloader code.
# Therefore, we need to flip the bit order around.
def compute_checksum(data):
    data = bytes(bitrev(byte, 8) for byte in data)
    checksum = binascii.crc32(data, 0)
    checksum = (checksum ^ 0xffffffff) & 0xffffffff
    return bitrev(checksum, 32)

def main(input_filename, output_filename):
    with open(output_filename, "wb") as output_file:
        with open(input_filename, "rb") as input_file:
            # First, we copy the input file into the output file.
            output_file.write(input_file.read())

            input_file.seek(0)
            output_file.seek(0)

            elf_file = elftools.elf.elffile.ELFFile(input_file)

            target_section = elf_file.get_section_by_name(".boot_2_flash")
            assert target_section.data_size == 256

            # We read the contents of the boot section into a buffer.
            data = bytearray(target_section.data())

            # We insert the checksum into the buffer.
            data[-4:] = struct.pack("<I", compute_checksum(data[:-4]))

            # We write the buffer into the output file.
            offset = target_section["sh_offset"]
            output_file.seek(offset)
            output_file.write(data)

if __name__ == "__main__":
    assert len(sys.argv) == 3
    main(sys.argv[1], sys.argv[2])
