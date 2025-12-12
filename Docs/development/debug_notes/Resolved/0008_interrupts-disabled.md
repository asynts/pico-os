commitid 9ce5f12a3af19ff09fdc638dafae7ab100c21b87

Somehow, we end up executing `wfi` with interrupts disabled.
Luckily, I placed an assertion there.

### Notes

-   This isn't the first time I am investigating this issue.

-   The `Kernel::are_interrupts_enabled` was implemented the wrong way around.

### Ideas

-   Verify the `PRIMASK` implementation.

### Theories

-   I suspect, that my implementation of the `PRIMASK` helpers don't work correctly.

### Actions

-   Change `return primask` to `return primask == 0`.
