#!/bin/bash
set -e

# Currently, this project is build with a custom bash script.
# In the future, it would be better to configure some build tool.

# FIXME: What combination of '-ffreestanding' '-nostdlib', '--specs' and '-nostartfiles' is actually necessary?
# FIXME: What about '-mthumb'

WARNINGS="-Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -Wpedantic -Wsign-conversion \
-Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wnull-dereference -Wuseless-cast -Wdouble-promotion"

ASM="arm-none-eabi-gcc"
ASMFLAGS="-mcpu=cortex-m0plus -mthumb -g $WARNINGS -I./Sources/boot/include -fno-exceptions -fno-rtti -nostdlib -ffreestanding"

CXX="arm-none-eabi-g++"
CXXFLAGS="-std=c++20 $WARNINGS -mcpu=cortex-m0plus -g -nostdlib -fmodules-ts -fno-exceptions -fno-rtti -ffreestanding -mthumb"

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
    "$CXX" $CXXFLAGS -o "Build/$filepath.o" -c "Sources/$filepath"

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

. Sources/build.sh

step_build_kit
step_build_kit_integers
step_build_kernel
step_build_boot

function step_link_system() {
    # FIXME: What precisely does '--specs=nosys.specs -nostartfiles' do?
    #        Do we need '-nostdlib' as well?

    arm-none-eabi-gcc \
        -static \
        -Wall -Wextra -mcpu=cortex-m0plus -g -mthumb -fno-exceptions -fno-rtti -ffreestanding -nostdlib \
        --specs=nosys.specs -nostartfiles \
        -Wl,--orphan-handling=error \
        -T Sources/link.ld \
        -o Build/System.elf \
        ${OBJS[@]}
}
step_link_system
