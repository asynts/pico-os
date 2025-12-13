#include <Kernel/Interface/System.hpp>
#include <Kernel/Threads/Scheduler.hpp>
#include <Kernel/Threads/Thread.hpp>
#include <Std/Format.hpp>

// Defined in Kernel/Threads/Scheduler.cpp
extern "C" Kernel::FullRegisterContext &
scheduler_next(Kernel::FullRegisterContext &context);

// Pico SDK panic
extern "C" void __attribute__((noreturn)) panic(const char *fmt, ...);

extern "C" {
// HardFault Handler (User Mode / Process context)
// Called when a fault occurs while the Process Stack Pointer (PSP) was in use.
Kernel::FullRegisterContext &
hard_fault_handler_cpp(Kernel::FullRegisterContext &context) {
  auto &scheduler = Kernel::Scheduler::the();
  auto thread = scheduler.get_active_thread_if_avaliable();

  if (thread) {
    dbgln("\n=== HardFault in User Thread ===");
    dbgln("Thread: '{}' (PID {})", thread->m_name,
          thread->m_process->m_process_id);

    // Dump registers
    dbgln("R0 ={x}  R1 ={x}  R2 ={x}  R3 ={x}", context.r0.m_storage,
          context.r1.m_storage, context.r2.m_storage, context.r3.m_storage);
    dbgln("R12={x}  LR ={x}  PC ={x}  PSR={x}", context.ip.m_storage,
          context.lr.m_storage, context.pc.m_storage, context.xpsr.m_storage);
    dbgln("SP ={}", &context); // Approximate PSP

    // Mark thread as masked so it won't be scheduled again, preventing a fault
    // loop
    thread->set_masked_from_scheduler(true);
  } else {
    // Should not happen if we came from PSP, unless scheduler state is
    // corrupted
    panic("HardFault in User Mode (PSP) but no active thread!");
  }

  // Switch to next scheduled thread
  return scheduler_next(context);
}

// HardFault Handler (Kernel Mode / IRQ context)
// Called when a fault occurs while Main Stack Pointer (MSP) was in use.
void kernel_hard_fault_handler_cpp(u32 *msp, u32 lr) {
  dbgln("\n=== HARD FAULT IN KERNEL MODE ===");
  dbgln("LR = {x} (EXC_RETURN)", lr);
  dbgln("MSP = {}", (void *)msp);

  // Cortex-M0+ pushes 8 words to the stack on exception:
  // [R0, R1, R2, R3, R12, LR, PC, xPSR]
  struct ExceptionStackFrame {
    u32 r0, r1, r2, r3, r12, lr, pc, xpsr;
  };

  auto *frame = reinterpret_cast<ExceptionStackFrame *>(msp);

  dbgln("Exception Stack Frame:");
  dbgln("  PC  = {x}", frame->pc);
  dbgln("  LR  = {x}", frame->lr);
  dbgln("  R0  = {x}", frame->r0);
  dbgln("  R1  = {x}", frame->r1);
  dbgln("  R2  = {x}", frame->r2);
  dbgln("  R3  = {x}", frame->r3);
  dbgln("  R12 = {x}", frame->r12);
  dbgln("  PSR = {x}", frame->xpsr);

  // TODO: On Cortex-M3/M4+ we would dump CFSR/HFSR here.
  // Cortex-M0+ has limited fault analysis capabilities.

  dbgln("Stack Trace (Raw Dump):");
  for (int i = 0; i < 16; ++i) {
    dbgln("  [{x}] = {x}", (u32)(msp + i), msp[i]);
  }

  panic("Kernel HardFault - System Halted");
}
}
