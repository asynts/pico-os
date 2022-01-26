#!/usr/bin/env python3

# Copies the input file and computes the checksum of the '.boot_2_flash_second_stage' section.
# The checksum is placed in the last four bytes of the output file.

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

# Originally, I wanted to patch the ELF file directly, but this requires that it is already padded appropriately.
# Now we simply print out an assembly file that can be compiled again.
# The disadvantage is, that we lose symbol information.
def generate_assembly(data, output_filename):
    assert len(data) == 256

    with open(output_filename, "w") as output_file:
        output_file.write(".cpu cortex-m0plus\n")
        output_file.write(".thumb\n")
        output_file.write("\n")
        output_file.write('.section .boot_2_flash_second_stage, "ax"\n')
        output_file.write("\n")

        for group_index in range(0, 256 // 16):
            output_file.write(".byte")

            prefix = " "
            for offset in range(0, 16):
                byte = data[group_index * 16 + offset]
                output_file.write(f"{prefix}{byte:#04x}")
                prefix = ", "
            output_file.write("\n")

def main(input_filename, output_filename):
    with open(input_filename, "rb") as input_file:
        elf_file = elftools.elf.elffile.ELFFile(input_file)

        target_section = elf_file.get_section_by_name(".boot_2_flash_second_stage")
        assert target_section.data_size <= 252

        # We read the contents of the boot section into a buffer.
        data = target_section.data()

        # We pad the buffer to 252 bytes.
        data += bytes(252 - len(data))

        # Append the checksum to the buffer.
        data += struct.pack("<I", compute_checksum(data[:-4]))

        generate_assembly(data, output_filename)

if __name__ == "__main__":
    assert len(sys.argv) == 3
    main(sys.argv[1], sys.argv[2])
