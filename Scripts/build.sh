#!/bin/bash
set -e

# Currently, this project is build with a custom bash script.
# In the future, it would be better to configure some build tool.

WARNING_FLAGS="-Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -Wpedantic -Wsign-conversion \
-Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wnull-dereference -Wuseless-cast -Wdouble-promotion"

FEATURE_FLAGS="-fno-exceptions -fno-rtti -ffreestanding -fmodules-ts"

COMMON_FLAGS="$WARNING_FLAGS $FEATURE_FLAGS -std=c++20 -mcpu=cortex-m0plus -mthumb -g --specs=nosys.specs"

CXX="arm-none-eabi-g++"
CXX_FLAGS="-I./Sources/boot/include"

LD="arm-none-eabi-g++"
LD_FLAGS="-Wl,--orphan-handling=error -nostartfiles -nodefaultlibs -nolibc -nostdlib -static"

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

# 'compile_asm' is an alias for this function.
function compile_cxx() {
    filepath=$1
    keep_or_discard=$2

    "$CXX" $COMMON_FLAGS $CXX_FLAGS \
        -o "Build/$filepath.o" \
        -c "Sources/$filepath"

    case "$keep_or_discard"
    in
    keep)
        OBJS+=("Build/$filepath.o")
        ;;
    discard)
        ;;
    *)
        echo "error: invalid 'keep_or_discard' option in 'compile_cxx'"
        exit 1
    esac
}

function compile_asm() {
    compile_cxx "$1" "$2"
}

source Sources/build.sh

function step_link() {
    "$LD" $COMMON_FLAGS $LD_FLAGS \
        -T Sources/link.ld \
        -o Build/System.elf \
        ${OBJS[@]}
}

step_build
step_link
