For some reason, when I try to load my trivial application (with the now correct checksum)
onto the chip it appears that it is not loaded.

### Notes

-   It seems that this chip isn't able to set hardware breakpoints in RAM.
    Therefore, GDB automatically inserts software breakpoints which may interfere with the loading routine.
    I tried the following:

    ```none
    load
    monitor reset init
    hbread *0x20000000
    ```

    ```none
    target halted due to debug-request, current mode: Thread
    xPSR: 0xf1000000 pc: 0x000000ee msp: 0x20041f00
    Error: Cortex-M Flash Patch Breakpoint rev.1 cannot handle HW breakpoint above address 0x1FFFFFFE
    Error: can't add breakpoint: unknown reason
    ```

-   I am not able to hit a breakpoint at address zero.
    Maybe this is used as a sentinel value, but I would like to stop in the early boot loader.

    However, I was able to stop at some other address.
    Maybe this is inside an interrupt handler because something went wrong?

-   When I load the `hello_serial.elf` executable, everything appears to work.
    It appears, tha tthe loading works as it should:

    ```none
    (gdb) load
    Loading section .boot2, size 0x100 lma 0x10000000
    Loading section .text, size 0x1f68 lma 0x10000100
    Loading section .rodata, size 0x15c lma 0x10002068
    Loading section .binary_info, size 0x28 lma 0x100021c4
    Loading section .data, size 0x5b4 lma 0x100021ec
    Start address 0x100001e8, load size 10144
    ```

    Compare this to:
    ```none
    Loading section .boot_2_flash_second_stage, size 0x100 lma 0x10000000
    Start address 0x20000000, load size 256
    ```

-   If I look at the executables with `readelf`, there are differences:

    ```none
    $ readelf --sections Build/System.elf
      [ 1] .boot_2_flas[...] PROGBITS        20000000 010000 000100 00 WAX  0   0  2
    ```

    ```none
    $ readelf --sections ~/src/github.com/raspberrypi/pico-examples/build/hello_world/serial/hello_serial.elf
      [ 1] .boot2            PROGBITS        10000000 001000 000100 00  AX  0   0  1
    ```

    In my opinion, my configuration is more correct then the other, since this code will be loaded at `0x20000000`.
    However, it doesn't work with the former.
    I suspect that `openocd` (incorrectly) looks at the virtual address instead of the load address?
    But it does appear to work when I inspect the memory manually?

-   Somehow, this is really inconsitent, if I repeat the following a few times:

    ```none
    load
    continue
    ```

    It sometimes works, I get the impression that this is related to the time between executing the two commands.
    When I place a breakpoint in `boot_2_flash_second_stage`, it appears to stop consistently.

-   It appears that linking the `.boot_2_flash_second_stage` section with `> FLASH` appears to work and links correctly for some reason.

    The machine code is as follows: `0d 21 00 be 30 bf fb e7`.
    The last two bytes are the `b` instruction: `1110011111111011`.

    The first five bits are the instruction prefix, `11100` for `b <imm11>`.
    If I read the manual correctly the remaining eleven bits are appended with a zero: `111111110110` and are sign extended
    to 32 bits: `11111111111111111111111111110110` which is `-10`.

-   What I truely do not understand is, why we are executing from `0x10000000`.
    My understanding is that the first boot stage should load the first 256 bytes into RAM and then jump there.
    How do we end up executing from flash memory?
    I've manually decoded the only jump instruction and it is fine.

### Ideas

-   What is the `monitor reset init` about?

### Theories

-   My theory is that I got FLASH and RAM the wrong way around.
    When we debug, we load the first 256 bytes into memory manually the bootloader isn't responsible for this.
    If we were to actually flush something onto the chip, that would be a different story.

-   I suspect, that `openocd` will simply put us at whatever is registered as entry address.
    We do not go through the boot loader.

### Conclusions
