commitid d64256e80cffded84981a8a3b8980b85eedfc095

We appear to return from the kernel after `sys$exit`.

### Notes

-   We crash here, in userspace:

    ```none
    #0  sys$exit (status=<optimized out>) at /home/me/dev/pico-os/Userland/LibC/sys/system.c:42
    #1  0x1001b348 in exit (status=-1) at /home/me/dev/pico-os/Userland/LibC/stdlib.c:15
    #2  0x1001b66c in ?? () at /home/me/dev/pico-os/Userland/LibC/sys/crt0.S:25
    ```

    This is an assertion I put there to ensure that `sys$exit` doesn't return:

    ```none
    void sys$exit(int status)
    {
        syscall(_SC_exit, status, 0, 0);
        asm volatile("bkpt #0");
        printf("sys$exit returned?\n");
        abort();
    }
    ```

-   When I fixed the `SystemHandler` code, I forgot about `exit` and that it needs special treatment.

### Actions

-   Do not add the thread back into the scheduler, on `_SC_exit`.
