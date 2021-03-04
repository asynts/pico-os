### Helpful Documentation

#### Stuff that I already looked at:

  - ARM ABI (https://github.com/ARM-software/abi-aa/releases)
  - ELF(5) man page (https://man7.org/linux/man-pages/man5/elf.5.html)

#### Stuff that I want to look at:

  - Blog about linkers (https://www.airs.com/blog/archives/38)

### Software

Installed via `pacman`:

~~~none
pacman -S --needed python-invoke
~~~

### Toolchain

I generally use the arm-none-eabi toolchain provided by Arch Linux.

However, the `arm-none-eabi-gdb` wasn't build with `--enable-targets=all` which means
that `add-symbol-file-from-memory` isn't avaliable. Therefor, I build my own GDB:

~~~none
../configure --prefix=/usr/local --enable-targets=all --target=arm-none-os-eabi
make -j8
~~~
