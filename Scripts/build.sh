#!/bin/bash
set -e

# Currently, this project is build with a custom bash script.
# In the future, it would be better to configure some build tool.

ASM="arm-none-eabi-gcc"
ASMFLAGS="-mcpu=cortex-m0plus -mthumb -g -Wall -Wextra -I./Sources/boot/include"

CXX="arm-none-eabi-g++"
CXXFLAGS="-std=c++20 -Wall -Wextra -mcpu=cortex-m0plus -g -nostdlib -fmodules-ts -fno-exceptions -fno-rtti"

LD="arm-none-eabi-ld"
LDFLAGS=""

OBJCOPY="arm-none-eabi-objcopy"

function trap_exit() {
    retval=$?
    if [[ $retval = 0 ]]
    then
        echo -e "\033[1;32msuccess\033[0m"
        exit 0
    else
        echo -e "\033[1;31mfailure\033[0m"
        exit $retval
    fi
}
trap trap_exit EXIT

if [[ "$PWD" != "/home/me/dev/pico-os" ]]
then
    echo "error: script executed in unknown location"
    exit 1
fi

OBJS=()

function compile_asm() {
    filepath=$1
    keep_or_discard=$2
    "$ASM" $ASMFLAGS -o "Build/$filepath.o" -c "Sources/$filepath"

    case "$keep_or_discard"
    in
    keep)
        OBJS+=("Build/$filepath.o")
        ;;
    discard)
        ;;
    *)
        echo "error: invalid 'keep_or_discard' option in 'compile_asm'"
        exit 1
    esac
}

function compile_cxx() {
    filepath=$1
    keep_or_discard=$2
    "$CXX" $CXXFLAGS -fno-exceptions  -o "Build/$filepath.o" -c "Sources/$filepath"

    case "$keep_or_discard"
    in
    keep)
        OBJS+=("Build/$filepath.o")
        ;;
    discard)
        ;;
    *)
        echo "error: invalid 'keep_or_discard' option in 'compile_asm'"
        exit 1
    esac
}

function step_build_boot() {
    [[ -d Build/boot ]] || mkdir -p Build/boot

    compile_asm "boot/boot_1_debugger.S" keep

    # First, we compile the assembly file.
    # Then, we pad it to 256 bytes with objcopy.
    # Finally we insert the checksum with a custom python script.
    #
    # This may seem overly complicated, but it retains all the debugging symbols.
    # We can't just pad the file itself with '.fill', because the data-in-code feature that ARM relies on, won't work.
    compile_asm "boot/boot_2_flash.S" discard
    # FIXME: Link here
    "$OBJCOPY" --gap-fill=0x00 --pad-to=0x100 "Build/boot/boot_2_flash.S.o" "Build/boot/boot_2_flash.padded.o"
    python3 Scripts/checksum.py Build/boot/boot_2_flash.padded.o Build/boot/boot_2_flash.patched.o
    OBJS+=("Build/boot/boot_2_flash.patched.o")

    compile_asm "boot/boot_3_reset.S" keep
    compile_asm "boot/boot_3_vectors.S" keep
}
step_build_boot

function step_link_system() {
    "$LD" $LDFLAGS -o Build/System.elf -T Sources/link.ld \
        ${OBJS[@]}
}
step_link_system
