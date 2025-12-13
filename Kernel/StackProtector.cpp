#include <Std/Format.hpp>
#include <stdint.h>

extern "C" [[noreturn]] void panic(const char *fmt, ...);

// Random value for the stack canary.
// Stack canary initialization is handled by init_stack_guard()
#include <hardware/regs/addressmap.h>
#include <hardware/regs/rosc.h>
#include <stddef.h>

extern "C" {
// Random value for the stack canary.
// Initialized to a magic value, then localized by init_stack_guard()
uintptr_t __stack_chk_guard = 0xDEADBEEF;

[[noreturn]] void __stack_chk_fail() {
  panic("Stack Smash Detected! (Canary corrupted)");
}
}

namespace Kernel {
void init_stack_guard() {
  // Use the ROSC (Ring Oscillator) RANDOMBIT register to generate a random
  // canary This is a simple hardware random source on the RP2040.

  volatile uint32_t *rosc_randombit =
      (volatile uint32_t *)(ROSC_BASE + ROSC_RANDOMBIT_OFFSET);
  uintptr_t random_canary = 0;

  for (size_t i = 0; i < sizeof(uintptr_t) * 8; ++i) {
    random_canary = (random_canary << 1) | (*rosc_randombit & 1);
    // Delay slightly to allow ROSC to fluctuate?
    // In practice, the loop overhead is enough.
  }

  // Ensure canary has null byte to terminate string overflows?
  // Common SSP tactic: 0x000aff0d (null, newline, etc).
  // For now, full random is fine, but let's ensure the lowest byte is non-zero
  // to avoid accidental null termination issues if we want to print it?
  // Actually, preventing string functions from reading PAST the canary is
  // better if the canary contains a nullptr. Linux uses 0 in the first byte for
  // 64-bit, logic is: strcpy stops at canary.

  // Let's set the LSB to 0 to stop string functions.
  random_canary &= ~0xFF;

  __stack_chk_guard = random_canary;

  dbgln("Stack Protection Initialized. Canary: {x}", __stack_chk_guard);
}
} // namespace Kernel
