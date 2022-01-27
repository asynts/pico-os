commit 8ad383d3f056d05cf24a5a919d9ca3fc7ab29f0e

There are some instructions which seem fine to me, but the assembler rejects them.
This is particularly annoying because many of these instructions are in the `pico-sdk` code.

### Notes

-   The following doesn't compile, but should:

    ```assembly
    #define A 0
    #define B 1

    ldr r1, =(A + B)
    ```

### Ideas

-   I could try to figure out how the SDK builds these files, this could reveal some compiler flag or something like that.

### Theories

-   I need to provide some compiler flag which I currently do not provide.

### Conclusions
