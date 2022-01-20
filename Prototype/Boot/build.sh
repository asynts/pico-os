#!/bin/bash
set -e

ASM="arm-none-eabi-as"
ASMFLAGS="-mcpu=cortex-m0plus"

LD="arm-none-eabi-ld"
LDFLAGS=""

[[ -d build ]] || mkdir -p build
"$ASM" $ASMFLAGS boot_2_flash_second_stage.S -o build/boot_2_flash_second_stage.S.o

python3 scripts/checksum.py build/boot_2_flash_second_stage.S.o build/boot_2_flash_second_stage.with-checksum.o

"$LD" $LDFLAGS -o build/system.elf -T link.ld \
    build/boot_2_flash_second_stage.with-checksum.o
