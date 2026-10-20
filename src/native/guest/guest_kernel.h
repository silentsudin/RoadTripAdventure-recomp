// W12 host EE libkernel (guest_kernel.cpp) — public frame-loop driver.
// The kernel runs guest threads as ucontext green threads on host stacks;
// all guest-visible state lives in rtguest::kRam / the thread table, and the
// scheduler switches only at syscall boundaries (no host threads, no timers):
// deterministic by construction. Semantics mirrored from the oracle
// ps2xRuntime EeScheduler (Syscalls/{Thread,Sync,Interrupt,System}.cpp):
// ready queues per priority (lower number = higher priority), thread ids
// 1 = boot thread / 2..255 allocated, KE_* error codes, sema wake result = id,
// SetVSyncFlag one-shot, alarms measured in libkernel 64 us ticks.
// shortcut: alarms and vblank fire at vblank-tick granularity (the oracle
// fires at EE-cycle deadlines); revisit at W13 hash parity if an alarm races
// a vblank boundary.
#pragma once
#include <cstdint>

namespace rtguest {

// One guest field (vblank start): +16667 us of virtual EE time, expired
// alarms invoke, SetVSyncFlag flag/tick write (one-shot), intc cause 0 fire.
void kVblank();

// Run every runnable green thread (and queued handler invocations) until all
// park again. Call after kVblank / any host-side wake from the frame loop.
void kPumpToIdle();

// Virtual EE microseconds (advanced only by kVblank); deterministic clock.
uint64_t kVirtualMicro();

// Current green thread id (0 while the pump host context runs). Handlers
// invoked by the pump run with the owner thread's id current, so the game's
// iWakeupThread wrappers see the caller-tid semantics of the real kernel.
int32_t kCurrentThreadId();

// Kernel object census for rt-app diagnostics / tests.
uint32_t kThreadCount();
uint32_t kSemaCount();

// Boot: install the green boot thread (id 1) for the ELF entry point and
// leave it ready; the frame loop then pumps per vblank.
void kBootMainThread(uint32_t entry, bool bootPhaseGate = true);

// Clear all kernel state (test isolation).
void kResetKernel();

// For host library services (guest_libs*): direct kernel-op entry points,
// mirroring how the oracle's ps2_stubs/ps2_syscalls call EeScheduler ops.
int32_t kSignalSema(int id, bool intr);
uint32_t kHeapAlloc(uint32_t bytes);  // oracle guestMalloc region (RPC dummy servers)
void kResetSifRegs();                 // oracle seedDefaultSifRegsLocked
void kSetCmdHandler(uint32_t cid, uint32_t fn, uint32_t arg);
void kEraseCmdHandler(uint32_t cid);
// Fire an installed dmac-cause handler chain (respects the EnableDmacHandler
// mask) — used by the host libdma stubs on transfer completion.
void kFireDmacCause(uint32_t cause);
// Park the calling green thread until the next kVblank (WaitVSyncTick mirror,
// Interrupt.cpp:78); kVblank resumes it and the call returns fixedResult.
int32_t kWaitVSync(int fixedResult);

}  // namespace rtguest
