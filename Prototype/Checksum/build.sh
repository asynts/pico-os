#!/bin/bash
set -e

# FIXME: This won't work, because the Assembly is ARM specifc and I am running
#        on an x86 machine.

ASM="as"
ASMFLAGS=""

CC="gcc"
CFLAGS=""

LD="ld"
LDFLAGS=""

[[ -d build ]] || mkdir build

"$ASM" $ASMFLAGS crc32.S -o build/crc32.S.o

"$CC" $CFLAGS -c main.c -o build/main.o

"$LD" $LDFLAGS -o build/checksum \
    build/main.o
    build/crc32.S.o
