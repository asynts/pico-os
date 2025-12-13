commitid 4a09ce9d17e5d8d8cabc7dabea3d8c8cf301215f

We crash when creating a new process.

### Notes

-   Now, we crash when creating a process, because I am calling `add_thread` without masking interrupts.

### Actions

-   I added a `MaskedInterruptGuard` thing when creating a new process.
