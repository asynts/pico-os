-   Clean up `PageAllocator` implementation.

-   Add `KernelMutex`, a passive syncronization primitive.

-   Make `String` immutable.

### Bugs

-   Ensure that we do not allocate or aquire locks in handler mode.

-   Ensure that `RefPtr` is only destoryed once.
