commmit cac3b549774228c1e82f4fefba5a99756c1bb574

I am having troubles when I have one module that imports another which is imported.
It appears that some parts are not transitive, and I can't quite make sense of it.
Not sure if this is a compiler bug, of if it's supposed to work this way.

### Notes

-   The following works:

    ```c++
    // foo.cpp

    export module x.foo;

    export namespace x {
        int foo = 42;
    }
    ```

    ```c++
    // bar.cpp
    export module x.bar;

    namespace x {
        export int bar = 13;
    }
    ```

    ```c++
    // baz.cpp

    import x.bar;
    import x.foo;

    void baz() {
        x::bar = 1;
    }
    ```

    ```none
    $ g++ -c -std=c++20 -fmodules-ts foo.cpp
    $ g++ -c -std=c++20 -fmodules-ts bar.cpp
    $ g++ -c -std=c++20 -fmodules-ts baz.cpp
    ```

-   However, the following doesn't work:

    ```c++
    // foo.cpp
    export module foo;

    import bar;

    namespace foo
    {
        export void foo() { }
    }
    ```

    ```c++
    // bar.cpp
    export module bar;

    namespace foo
    {

    }
    ```

    ```c++
    // baz.cpp
    export module baz;

    import foo;
    import bar;

    int main() {
        foo::foo();
    }
    ```

    ```bash
    #!/bin/bash
    set -e

    g++ -std=c++20 -fmodules-ts -c bar.cpp
    g++ -std=c++20 -fmodules-ts -c foo.cpp
    g++ -std=c++20 -fmodules-ts -c baz.cpp
    g++ -std=c++20 -fmodules-ts foo.o bar.o baz.o
    ```

    However, if the empty namespace in `bar.cpp` is removed, it now compiles fine.

### Ideas

-   I should consider moving on without C++20 modules, the feature doesn't appear to be ready yet.

-   I could try to reproduce this with the "trunk" version of GCC.

### Theories

-   This is a serious compiler bug.

### Conclusions
