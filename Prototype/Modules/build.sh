#!/bin/bash
set -e

# This seems to be fairly unstable, sometimes I get the following error message:
# "error: import ‘common:baz’ has CRC mismatch"
# This seems to happen when I introduce a circular dependency.
#
# On more though, I think this was because I compiled the modules in the incorrect order.
# 'common:foo' imported 'common:baz' but they were compiled the other way around.
# Thus we grabed the older version.

# My conclusion is that I can probably start using this.
# If it doesn't work, I can simply go back to using headers.
# The question is if it would actually compile faster this way, since I invoke only one compiler instance at a time.
# However, this project is small enough and that won't matter.

CXX="arm-none-eabi-g++"
CXXFLAGS="-std=c++20 -fmodules-ts -nostdlib -mcpu=cortex-m0plus"

[[ -d build/common ]] || mkdir -p build/common
"$CXX" $CXXFLAGS -c common/baz.cpp -o build/common/baz.cpp.o
"$CXX" $CXXFLAGS -c common/foo.cpp -o build/common/foo.cpp.o
"$CXX" $CXXFLAGS -c common/bar.cpp -o build/common/bar.cpp.o
"$CXX" $CXXFLAGS -c common/module.cpp -o build/common/module.cpp.o

[[ -d build/std ]] || mkdir -p build/std
"$CXX" $CXXFLAGS -c std/module.cpp -o build/std/module.cpp.o

[[ -d build ]] || mkdir -p build
"$CXX" $CXXFLAGS -c main.cpp -o build/main.cpp.o
"$CXX" -std=c++20 -o build/main \
    build/common/foo.cpp.o \
    build/common/bar.cpp.o \
    build/common/baz.cpp.o \
    build/common/module.cpp.o \
    build/std/module.cpp.o \
    build/main.cpp.o
