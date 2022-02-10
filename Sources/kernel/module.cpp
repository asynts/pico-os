export module kernel;

namespace kernel {
    export
    void entry() {
        asm volatile("bkpt #0");
    }
}
