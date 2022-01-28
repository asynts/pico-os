#!/bin/bash
set -e

# Currently, this project is build with a custom bash script.
# In the future, it would be better to configure some build tool.

ASM="arm-none-eabi-gcc"
ASMFLAGS="-mcpu=cortex-m0plus -mthumb -g -Wall -Wextra -I./Sources/boot/include -fno-exceptions -fno-rtti -nostdlib"

CXX="arm-none-eabi-g++"
CXXFLAGS="-std=c++20 -Wall -Wextra -mcpu=cortex-m0plus -g -nostdlib -fmodules-ts -fno-exceptions -fno-rtti"

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
    # Then we link it to ensure that all relocations are gone and that we have exactly 256 bytes.
    # Finally we insert the checksum with a custom python script.
    compile_asm "boot/boot_2_flash.S" discard
    python3 Scripts/checksum.py Build/boot/boot_2_flash.S.o Build/boot/boot_2_flash.patched.o
    OBJS+=("Build/boot/boot_2_flash.patched.o")

    compile_asm "boot/boot_3_vectors.S" keep
}
step_build_boot

function step_link_system() {
    # FIXME: What precisely does '--specs=nosys.specs -nostartfiles' do?
    #        Do we need '-nostdlib' as well?

    arm-none-eabi-gcc \
        -nostdlib -fno-exceptions -fno-rtti \
        --specs=nosys.specs -nostartfiles \
        -T Sources/link.ld \
        -o Build/System.elf \
        ${OBJS[@]}
}
step_link_system
