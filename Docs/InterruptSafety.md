# Interrupt Safety in PicoOS

This document outlines the safety of calling various kernel functions and using synchronization primitives from within Interrupt Service Routines (ISRs).

## Synchronization Primitives

| Primitive | ISR Safe? | Notes |
| :--- | :--- | :--- |
| `HardwareSpinLock` | **YES** | Safe to use. Note that it does **not** disable interrupts locally, so care must be taken to avoid deadlocks if key resources are shared with the thread on the *same* core. |
| `SoftwareSpinLock` | **YES** | Safe. Uses local interrupt masking to preventing recursion on the same core. |
| `SoftwareMutex` | **NO** | Passive locking. May sleep/yield, which is illegal in an ISR. |
| `MaskedInterruptGuard` | **N/A** | Used to disable interrupts. Safe to construct in ISR (no-op or redundant), but intended for Thread mode. |

## Memory Allocation

| Function | ISR Safe? | Notes |
| :--- | :--- | :--- |
| `kmalloc` / `kfree` | **NO** | Uses `malloc_mutex` (`SoftwareMutex`). |
| `PageAllocator::allocate` | **NO** | Uses `page_allocator_mutex` (`SoftwareMutex`). |
| `GlobalMemoryAllocator` | **NO** | Uses `malloc_mutex`. |
| `MemoryAllocator::allocate_eternal` | **NO** | Uses `malloc_mutex`. |

**Recommendation**: Perform all memory allocation in Thread mode. If an ISR needs memory, pre-allocate it in the initialization phase or use a lock-free structure (e.g., ring buffer) populated by a worker thread.

## Logging

| Function | ISR Safe? | Notes |
| :--- | :--- | :--- |
| `dbgln` | **YES** | Uses `dbgln_mutex`. *Wait*, checking implementation... `dbgln` uses `KernelMutex` which is `SoftwareMutex`. **NO**, it is not safe if it blocks. logic might deadlock if interrupted thread holds the lock. |
| `kprintf` | **NO** | Same as `dbgln`. |

**Warning**: Using `dbgln` in an ISR while the main thread is printing can cause a deadlock. Use with caution or only for fatal errors (`panic` overrides locks).

## Scheduler

| Function | ISR Safe? | Notes |
| :--- | :--- | :--- |
| `Scheduler::trigger` | **YES** | Safe. Sets PendSV bit to request a schedule. |
| `Scheduler::schedule` | **NO** | Should only be called by `pendsv` handler. |
| `Thread::wakeup` | **YES** | Safe. Adds thread to queue and triggers scheduler. |

## General Rules

1.  **Do not block**: Never use blocking synchronization (Mutex, Semaphores) in an ISR.
2.  **Do not allocate**: Avoid heap operations.
3.  **Quick execution**: Keep ISRs short. Offload work to `WorkerThread` or unblock a driver thread.
