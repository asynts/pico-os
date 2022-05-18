commitid 1c006cdf0ee197ff5c560158fdde5b74cf7a2262

There are tons of open problems that need to be resolved.

### Ideas

-   Verify that we do not allocate in a interrupt handlers.

-   I should add an assertion that checks that if we schedule away from something, it can't be holding any locks.

-   I should use the builtin mutex things from the processor.

-   I really need to cut out the Pico SDK thing entirely.
    The whole memory allocation thing is a mess.

-   Essentially, the `KernelMutex` was my attempt at multi-core support, however, they don't really do anything,
    other than to cause trouble.

    I need more sophisticated debugging tools to be able to deal with the whole multi-core ordeal.
