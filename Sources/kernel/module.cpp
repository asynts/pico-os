export module kernel;

import kit;

using namespace kit::prelude;

namespace kernel {
    export
    void entry() {
        asm volatile("bkpt #0");
    }
}
