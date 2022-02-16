commit 0e28d39deb86d828623c175f41f43900a95fab7f

### Notes

-   Implementing a GDB stub seems trivial:

    >   At a minimum, a stub is required to support the ‘?’ command to tell gdb the reason
    >   for halting, ‘g’ and ‘G’ commands for register access, and the ‘m’ and ‘M’ commands for
    >   memory access. Stubs that only control single-threaded targets can implement run control
    >   with the ‘c’ (continue) command, and if the target architecture supports hardware-assisted
    >   single-stepping, the ‘s’ (step) command. Stubs that support multi-threading targets should
    >   support the ‘vCont’ command. All other commands are optional.

-   I think I need to implement this in C or C++, because I need `libusb` to communicate with the probe.
    There appears to be a python wrapper library but I can't imagine it is any good.

-   The RP2040 chip implements the ADI v5.1 protocol.
    https://developer.arm.com/documentation/ihi0031/latest

### Ideas

-   What configuration needs to be provided to the debugger about the system itself?

-   What primitives does the GDB stub need?

-   I need to research, how SWD works and if it's feasible to implement.
    Maybe there are existing projects that can be ported?

-   Read: ARM Debug Interface ADIv5.1 https://developer.arm.com/documentation/ihi0031/latest

    -   Chapter A1: About the Arm Debug Interface

    -   Chapter B1 About the DP

    -   Chapter B4 The Serial Wire Debug Port (SW-DP)

    -   Chapter C1 About the AP

### Theories

### Conclusions
