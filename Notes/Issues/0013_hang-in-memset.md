commit c685dfd26bf90613467e6e82a78ec317b27dc084

We appear to load something sensible for the `memset_ptr` address, but when trying to execute it, we hang somewhere.

### Notes

### Ideas

### Theories

-   I suspect, that the thumb bit is not set.

-   I suspect, that we are reading the wrong function pointer.

### Conclusions
