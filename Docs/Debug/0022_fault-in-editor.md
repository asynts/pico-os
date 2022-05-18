When we use the `a` command in `Editor.elf`, we hang and then overflow the stack.

### Notes

-   This is only the end of the stacktrace, the `memmove` calls continue.

    ```none
    #521 0x1001c7c0 in memmove (dest=0x2001001a, src=0x20010024, count=536936440) at /home/me/dev/pico-os/Userland/LibC/string.c:82
    #522 0x1001c7c0 in memmove (dest=0x2001001a, src=0x20010024, count=536936440) at /home/me/dev/pico-os/Userland/LibC/string.c:82
    #523 0x1001d044 in buffer_append_at_offset (buf=0x200120e0, offset=0, data=0x20010318 "\301\307\001\020\032", data_size=2) at /home/me/dev/pico-os/Userland/Editor.c:23
    #524 main (argc=<optimized out>, argv=<optimized out>) at /home/me/dev/pico-os/Userland/Editor.c:211
    #525 0x1001c950 in ?? () at /home/me/dev/pico-os/Userland/LibC/sys/crt0.S:19
    ```

### Theories

-   I suspect, that the compiler recognized my `memmove` implementation and replaced it with a call to itself.
    That happened many times before.
