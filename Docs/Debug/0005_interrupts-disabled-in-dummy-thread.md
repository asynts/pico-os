commitid 1e6e5ae5e674aa4032153bd07037ea1226b1e6ba

Somehow, we end up switching to the dummy thread with interrupts disabled.

### Notes

-   My understanding is, that the dummy thread should die immediately?

### Theory

-   I suspect, that the `KernelMutex` is keeping the thread alive somehow.
