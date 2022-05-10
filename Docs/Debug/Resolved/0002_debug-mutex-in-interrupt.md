commitid 77a87c4077d5c17f94ebeb926655de89c457da18

We appear to call `Kernel::KernelMutex::unlock` in the PendSV interrupt
handler.

### Notes

-   This appears to be caused by the destructor of `Kernel::Thread` which calls
    `dbgln`.

### Ideas

-   We should not interact with the mutex in the interrupt handler, even if
    this messes up the output.  We may have to do other syncronization though.

    Maybe we should not write debug messages in interrupt handlers for now.

### Changes

-   Ignore debug messages in handler mode and print message at next
    oppurtunity.

    91166f657e0a0643091b8c2af95d621c06807895
