#!/bin/bash
set -e

[[ -d build/common ]] || mkdir -p build/common
g++ -std=c++20 -fmodules-ts -c common/foo.cpp -o build/common/foo.cpp.o
g++ -std=c++20 -fmodules-ts -c common/bar.cpp -o build/common/bar.cpp.o
g++ -std=c++20 -fmodules-ts -c common/module.cpp -o build/common/module.cpp.o
g++ -std=c++20 -fmodules-ts -c main.cpp -o build/main.cpp.o

[[ -d build ]] || mkdir -p build
g++ -std=c++20 build/common/foo.cpp.o build/common/bar.cpp.o build/common/module.cpp.o build/main.cpp.o -o build/main
