commit c685dfd26bf90613467e6e82a78ec317b27dc084

We appear to load something sensible for the `memset_ptr` address, but when trying to execute it, we hang somewhere.

### Notes

-   Somehow, `memset` recursively called itself.
    My takeaway was, that the compiler tries to be smart about it.

-   It appears that the compiler does not make such assumptions about the `__aeabi_*` functions.
    I provided implementations for these instead.

### Ideas

### Theories

### Conclusions

-   The compiler tried to be smart about `memset` and got confused because it tried to use `memset` in `memset`.
