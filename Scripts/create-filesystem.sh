#!/bin/bash
set -e

root=/mnt/EmbeddedFileSystem

truncate --size 300K FileSystem.img
mkfs.ext2 -q FileSystem.img
[[ -d $root ]] || sudo mkdir $root
sudo mount -o loop FileSystem.img $root

[[ ! -e Root ]] || rm -r Root
sudo cp Shell.elf $root/

sudo umount FileSystem.img

arm-none-eabi-objcopy -I binary -O elf32-littlearm FileSystem.img FileSystem.1.elf
arm-none-eabi-objcopy \
    --redefine-sym _binary_FileSystem_img_start=__fs_start \
    --redefine-sym _binary_FileSystem_img_end=__fs_end \
    --redefine-sym _binary_FileSystem_img_size=__fs_size \
    FileSystem.1.elf FileSystem.elf
