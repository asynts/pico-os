export module kernel;

export import workaround;

// We have to import this here because of a compiler bug in GCC.
// It is unable to find the placement 'new operator'.
//
// Usually, this shouldn't matter, because it is avaliable in the 'kit' module, however,
// it appears that GCC treats this operator special, presumably, because it is parsed that way.
import kit;

import kernel.drivers;

namespace kernel
{
    // This is the entry point of the kernel.
    // At this point this is essentially a normal application.
    // The bootloader loaded the kernel into memory and called the global constructors.
    export
    void entry() {
        drivers::UartDriver::initialize();

        asm volatile("bkpt #0");
    }
}

export void whatever() {
    kernel::entry();
}
