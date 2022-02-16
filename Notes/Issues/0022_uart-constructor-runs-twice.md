commit 4289d970c871e331a260519428d74a4da99263e9

Somehow, the constructor from `UartOutputDriver` is executed twice.

### Notes

-   When I set a breakpoint at `UartOutputDriver::UartOutputDriver()`, this breakpoint is hit twice.

-   In theory, the `Singleton<UartOutputDriver>::m_initialized` should prevent this from happening.
    There are no two calls in the code either.

### Ideas

-   Try adding `volatile` to `m_initialized`.

-   Read the documentation about the boot loader.

### Theories

-   Maybe the boot loader also sends CORE1 into `boot_3_vectors`, if they both run at exactly the same pace, it
    would be possible that we have a race condition.

### Conclusions
