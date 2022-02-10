export module kernel;

import kernel.drivers;

namespace kernel {
    // This is the entry point of the kernel.
    // At this point this is essentially a normal application.
    // The bootloader loaded the kernel into memory and called the global constructors.
    export
    void entry() {
        drivers::UartDriver::initialize();

        asm volatile("bkpt #0");
    }
}
