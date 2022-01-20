#!/bin/bash
set -e

# This seems to be fairly unstable, sometimes I get the following error message:
# "error: import ‘common:baz’ has CRC mismatch"
# This seems to happen when I introduce a circular dependency.

# My conclusion is that I can probably start using this.
# If it doesn't work, I can simply go back to using headers.
# The question is if it would actually compile faster this way, since I invoke only one compiler instance at a time.
# However, this project is small enough and that won't matter.

[[ -d build/common ]] || mkdir -p build/common
g++ -std=c++20 -fmodules-ts -c common/foo.cpp -o build/common/foo.cpp.o
g++ -std=c++20 -fmodules-ts -c common/bar.cpp -o build/common/bar.cpp.o
g++ -std=c++20 -fmodules-ts -c common/baz.cpp -o build/common/baz.cpp.o
g++ -std=c++20 -fmodules-ts -c common/module.cpp -o build/common/module.cpp.o
g++ -std=c++20 -fmodules-ts -c main.cpp -o build/main.cpp.o

[[ -d build ]] || mkdir -p build
g++ -std=c++20 -o build/main \
    build/common/foo.cpp.o \
    build/common/bar.cpp.o \
    build/common/baz.cpp.o \
    build/common/module.cpp.o \
    build/main.cpp.o
