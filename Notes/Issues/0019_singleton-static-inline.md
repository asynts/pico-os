commit cac3b549774228c1e82f4fefba5a99756c1bb574

The `m_initialized` static member isn't defined anywhere, my understanding is that `inline` should resolve this issue.
Maybe another compiler bug?

### Notes

-   ```none
    CXX Sources/boot/boot_4_load_kernel.cpp -> Build/boot/boot_4_load_kernel.cpp.o
    arm-none-eabi/bin/ld: Build/kernel/module.cpp.o: in function `kit::Singleton<kernel::drivers::UartDriver>::initialize()':
    Sources/kit/module.cpp:57: undefined reference to `kit::Singleton<kernel::drivers::UartDriver>::m_initialized'
    collect2: error: ld returned 1 exit status
    ```

### Theories

-   I seem to recall working around another similar issue when implementing `Singleton` before.

-   This might be another compiler bug.

### Conclusions
