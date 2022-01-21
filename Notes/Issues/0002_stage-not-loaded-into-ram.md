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

### Ideas

-   I could try to create an application with the SDK and try to debug that.

-   What is the `monitor reset init` about?

### Theories

### Conclusions
