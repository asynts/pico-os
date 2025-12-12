commitid a0049d081678b4376cd65d12b9fb8ad3156725bd

### Notes

-   I tried disabling the interrupt stuff but that did not help.

-   I was unconditionally calling `delete[]` in `~Vector` even if the inline data was used.

### Theories

-   I suspect, that the `restore_interrupts` or the `disable_interrupts` call isn't working properly.

### Actions

-   I only call `delete[]` in `~Vector` if a buffer actually exists.
