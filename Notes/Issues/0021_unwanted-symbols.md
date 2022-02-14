commit 308a8b933009d6f2f168661b2d1e02a2ee56ffb2

### Notes

-   There are a few symbols in `System.elf` that shouldn't be there in my opinion:

    -   ```none
        _GLOBAL__sub_I_memcpy_ptr
        ```

        I don't know what this is in the first place, but the `memcpy_ptr` variable should not be initialized at all.
        That's why it was put into the `.noinit` section.

    -   ```none
        guard variable for Kit::Singleton<Kernel::Drivers::UartDriver>::m_value
        ```

        What the hell is that?
        That better not be a mutex.

### Ideas

-   I should verify that `memcpy_ptr` is still intact after the `.init_array` constructors ran.

-   Most of this shouldn't matter and can just be moved into a ticket with low priority.

### Theories

### Conclusions
