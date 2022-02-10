commit 2305fd552dc9699224e0c0070a439d1dff772ff2

I am working on implementing the UART driver and need to import the `Singleton` class I wrote before.
However, I can not use the placement `new` operator for some reason.

### Notes

-   The following code complains:

    ```c++
    static void initialize() {
        if (m_initialized) {
            assert_not_reached();
        } else {
            new (&m_value) T;
            m_initialized = true;
        }
    }
    ```

    ```none
    In module kit, imported at Sources/kernel/drivers/module.cpp:3,
    of module kernel.drivers, imported at Sources/kernel/module.cpp:3:
    Sources/kit/module.cpp: In instantiation of 'static void kit::Singleton@kit<T>::initialize() [with T = kernel::drivers::UartDriver@kernel.drivers]':
    Sources/kernel/module.cpp:11:30:   required from here
    Sources/kit/module.cpp:58:17: error: no matching function for call to 'operator new(sizetype, kernel::drivers::UartDriver@kernel.drivers*)'
    58 |                 new (&m_value) T;
        |                 ^~~~~~~~~~~~~~~~
    <built-in>: note: candidate: 'void* operator new(unsigned int)'
    <built-in>: note:   candidate expects 1 argument, 2 provided
    <built-in>: note: candidate: 'void* operator new(unsigned int, std::align_val_t)'
    <built-in>: note:   no known conversion for argument 2 from 'kernel::drivers::UartDriver@kernel.drivers*' to 'std::align_val_t'
    Sources/kernel/module.cpp:1:8: warning: not writing module 'kernel' due to errors
        1 | export module kernel;
        |        ^~~~~~
    ```

-   Now I am receiving this error, not sure what I am doing differently:

    ```none
    Sources/boot/boot_4_load_kernel.cpp: In function 'void boot_4_load_kernel()':
    Sources/boot/boot_4_load_kernel.cpp:99:5: error: 'kernel' has not been declared
    99 |     kernel::entry();
        |     ^~~~~~
    Sources/boot/boot_4_load_kernel.cpp: At global scope:
    Sources/boot/boot_4_load_kernel.cpp:1:8: warning: not writing module 'boot' due to errors
        1 | export module boot;
        |        ^~~~~~
    ```

    Actually, the difference was that I added `export` to `operator new`:

    ```c++
    export
    inline void* operator new(kit::usize, void *pointer) {
        return pointer;
    }
    ```

-   I tried commenting that out and it compiles again.

-   I noticed that these weird symbols also appear for imports:

    ```none
             U _ZGIW6kernel7driversEv
    0000000d T _ZGIW6kernelEv
    ```

    Maybe they are used to pull in all the dependencies.
    This way the linker can't discard the global constructors of that other module.

-   I rebased my code a bit and now the issue is gone, if this was a compiler issue this will surely come back.

### Ideas

### Threories

### Conclusions

-   I had to export the `new` operator, which seems a bit weird but whatever.
    In my understanding, this should work as is.
    But maybe the `new` operator is a special case in the compiler somehow.
