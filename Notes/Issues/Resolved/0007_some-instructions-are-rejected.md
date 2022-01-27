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

-   When it is compiled in the SDK, this is done as follows:

    ```none
    arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -c compile_time_choice.S -o compile_time_choice.S.obj
    ```

    I removed all the clutter around it.

### Ideas

### Theories

### Conclusions

-   In the SDK, instead of using `arm-none-eabi-as`, they use `arm-none-eabi-gcc -c`.
    Weird, but I'll do the same.
