commitid a0f56d6fd445073e8ce100f2433a555ed0e9b6bf

After startup, we print out the shell prompt and then run out of memory.

### Notes

-   The memory allocation occurs in the `write()` system call in
    `String::format`.

-   All allocations are very small, so we are not running out of memory any
    time soon.  We startup with 16KiB of heap which is a lot for this system.

-   After turning on the debug mode of the memory allocator, we run into an
    assertion in the scheduler.  This is debugged seperately in
    `Docs/Debug/0002_*`.

-   It appears that we are allocating in `dbgln` when calling
    `Kernel::syscall`.

    This is not ok, because the implementation is not able to deal with
    interrupts.

### Ideas

-   Try running the test suite.

-   Which memory allocation can not be fulfilled?

-   Go through the commit history.

-   XXX Can we avoid allocating in interrupts?

-   XXX We could push another stack frame and then return from the interrupt
    prematurely.  This will be insecure if this part is writable by another
    thread in the same process, but we are not there yet.

### Theories

-   Maybe we allocate from an interrupt handler and this is an syncronization
    issue.

-   We have some sort of race condition and my very fast computer runs into it
    consistently.

-   There is something that allocates stuff in an infinite loop.

-   We do not have enough memory avaliable because of `pico-sdk`.

-   In deallocate, we sometimes drop tons of memory.
