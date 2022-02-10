export module kernel;

import kit;

using namespace kit::prelude;

namespace kernel {
    // This is the entry point of the kernel.
    // At this point this is essentially a normal application.
    // The bootloader loaded the kernel into memory and called the global constructors.
    export
    void entry() {
        asm volatile("bkpt #0");
    }
}
