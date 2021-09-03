commitid a439eb91d48f99afa71bdefaa1742fedfd9fd868

We end up in the default thread with interrupts disabled.

### Notes

### Ideas

-   Currently, we only use a single core, so we should be able to drop most
    mutexes.

-   Verify the `PRIMASK` implementation.

### Theories

-   I suspect, that my implementation of the `PRIMASK` helpers don't work
    correctly.
