// W12 kernel pins: the host libkernel (guest_kernel.cpp) exercised through the
// real syscall-stub TUs + the ucontext green-thread pump. Every expectation is
// pinned to oracle ps2xRuntime EeScheduler semantics (return-id sema results,
// caller-tid GetThreadId, priority order, one-shot vsync flag, alarm args).
#include "guest/guest_kernel.h"
#include "guest/guest_shim.h"

#include <cstdio>
#include <cstring>

namespace guest {
int32_t CreateSema(uint32_t); int32_t DeleteSema(uint32_t);
int32_t SignalSema(uint32_t); int32_t PollSema(uint32_t); int32_t WaitSema(uint32_t);
int32_t CreateThread(uint32_t); int32_t StartThread(uint32_t, uint32_t);
int32_t GetThreadId(); int32_t SleepThread(); int32_t WakeupThread(uint32_t);
int32_t ExitDeleteThread();
int32_t ReferThreadStatus(uint32_t, uint32_t); int32_t SetVSyncFlag(uint32_t, uint32_t);
int32_t SetAlarm(uint32_t, uint32_t, uint32_t);
int32_t syscallMemInstall(uint32_t, uint32_t, uint32_t);
uint32_t iWakeupThread(uint32_t, ...);  // game libkernel wrapper (00285ed8)
}

using namespace rtguest;

static int fails = 0;
static void check(bool ok, const char* what, int line) {
    if (!ok) { std::fprintf(stderr, "FAIL %s (line %d)\n", what, line); ++fails; }
}
#define CHECK(cond) check((cond), #cond, __LINE__)

static void wr32(uint32_t a, uint32_t v) { std::memcpy(G(a), &v, 4); }
static uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, G(a), 4); return v; }
static uint64_t rd64(uint32_t a) { uint64_t v; std::memcpy(&v, G(a), 8); return v; }

// ee_thread_t at addr: func, stack, size, gp, prio.
static void makeTh(uint32_t addr, uint32_t func, uint32_t size, int prio) {
    wr32(addr + 0x04, func); wr32(addr + 0x08, 0); wr32(addr + 0x0C, size);
    wr32(addr + 0x10, 0); wr32(addr + 0x14, (uint32_t)prio);
}

// ---- green entry bodies (registered as guest functions) ----

static uint64_t t1_body(uint64_t, uint64_t, uint64_t, uint64_t) {
    wr32(0x400000, (uint32_t)guest::GetThreadId());
    const int32_t r = guest::WaitSema(rd32(0x400020));
    wr32(0x400004, (uint32_t)r);
    guest::ExitDeleteThread();
    return 0;
}

static uint64_t t_prio_body(uint64_t slot, uint64_t, uint64_t, uint64_t) {
    wr32(0x400400, rd32(0x400400) + 1); wr32((uint32_t)slot, rd32(0x400400));
    guest::SleepThread();
    wr32(0x400400, rd32(0x400400) + 1); wr32((uint32_t)slot + 4, rd32(0x400400));
    guest::ExitDeleteThread();
    return 0;
}

// iWakeupThread on self: must take the libkernel ring path (00285ed8).
static uint64_t t_ring_body(uint64_t, uint64_t, uint64_t, uint64_t) {
    const uint32_t self = (uint32_t)guest::GetThreadId();
    guest::iWakeupThread(self);
    guest::ExitDeleteThread();
    return 0;
}

// iWakeupThread on another thread: bare-call fix must forward the id.
static uint64_t t_wake_other_body(uint64_t target, uint64_t, uint64_t, uint64_t) {
    guest::iWakeupThread((uint32_t)target);
    guest::ExitDeleteThread();
    return 0;
}

static uint64_t alarm_handler(uint64_t a0, uint64_t a1, uint64_t a2, uint64_t) {
    wr32(0x400500, (uint32_t)guest::GetThreadId());
    wr32(0x400504, (uint32_t)a0);   // alarm id
    wr32(0x400508, (uint32_t)a1);   // ticks
    wr32(0x40050C, (uint32_t)a2);   // registered argument
    return 0;
}

static uint64_t intc_handler(uint64_t a0, uint64_t a1, uint64_t, uint64_t) {
    wr32(0x400700, (uint32_t)a0);   // cause
    wr32(0x400704, (uint32_t)a1);   // handler argument
    return 0;
}

static uint64_t t_sleep_body(uint64_t, uint64_t, uint64_t, uint64_t) {
    guest::SleepThread();
    wr32(0x400900, rd32(0x400900) + 1);
    guest::ExitDeleteThread();
    return 0;
}

// boot main thread: records its thread id, parks one SleepThread, records again.
static uint64_t t_boot_body(uint64_t, uint64_t, uint64_t, uint64_t) {
    wr32(0x400A00, (uint32_t)guest::GetThreadId());
    guest::SleepThread();
    wr32(0x400A04, 1);
    guest::SleepThread();
    return 0;
}

// WaitVSyncTick park (what sceGsSyncV's host body does on a green thread).
static uint64_t t_vsync_body(uint64_t, uint64_t, uint64_t, uint64_t) {
    wr32(0x400B00, (uint32_t)rtguest::kWaitVSync(7));
    wr32(0x400B04, (uint32_t)rtguest::kWaitVSync(-1));
    guest::ExitDeleteThread();
    return 0;
}

int main() {
    kResetKernel();
    std::memset(kRam, 0, 0x500000);  // scratch zone for all pins

    // 1) sema lifecycle through the stubs (host context, no blocking)
    wr32(0x400100 + 0x04, 2);  // ee_sema_t max_count
    wr32(0x400100 + 0x08, 1);  // init_count
    const int32_t semaA = guest::CreateSema(0x400100);
    CHECK(semaA == 1);
    CHECK(guest::PollSema((uint32_t)semaA) == semaA);
    CHECK(guest::PollSema((uint32_t)semaA) == -419);  // KE_SEMA_ZERO
    CHECK(guest::SignalSema((uint32_t)semaA) == semaA);
    CHECK(guest::SignalSema((uint32_t)semaA) == semaA);
    CHECK(guest::SignalSema((uint32_t)semaA) == -420);  // KE_SEMA_OVF
    CHECK(guest::DeleteSema((uint32_t)semaA) == semaA);
    CHECK(guest::PollSema((uint32_t)semaA) == -408);    // KE_UNKNOWN_SEMID
    CHECK(guest::CreateThread(0) == -1);                // KE_ERROR
    makeTh(0x400110, 0x0F000100, 0x400, 0);             // priority 0 is illegal
    CHECK(guest::CreateThread(0x400110) == -403);       // KE_ILLEGAL_PRIORITY
    makeTh(0x400110, 0x0F000100, 0x400, 2);
    CHECK(guest::CreateThread(0x400110) == 2);          // first id is 2
    CHECK(guest::CreateThread(0x400110) == 3);
    CHECK(guest::DeleteSema(999) == -408);

    // 2) green thread: sema wait → wake result = sema id (oracle wake result)
    kResetKernel();
    std::memset(kRam, 0, 0x500000);
    wr32(0x400120 + 0x04, 1); wr32(0x400120 + 0x08, 0);
    const int32_t semaB = guest::CreateSema(0x400120);
    wr32(0x400020, (uint32_t)semaB);
    registerGuest(0x0F000100, (void*)&t1_body);
    makeTh(0x400110, 0x0F000100, 0x400, 2);
    CHECK(guest::CreateThread(0x400110) == 2);
    CHECK(guest::StartThread(2, 0x1122) == 0);
    kPumpToIdle();
    CHECK(rd32(0x400000) == 2);              // GetThreadId inside green thread
    CHECK(kThreadCount() == 1);
    CHECK(rd32(0x400004) == 0);              // still parked in WaitSema
    CHECK(guest::SignalSema((uint32_t)semaB) == semaB);
    kPumpToIdle();
    CHECK(rd32(0x400004) == (uint32_t)semaB);  // waiter's WaitSema returned id
    CHECK(kThreadCount() == 0);                // ExitDeleteThread reclaimed it

    // 3) priority order + WakeupThread; and the game iWakeupThread wrapper
    //    forwarding to _iWakeupThread (bare-call fix, 00285ed8 else-branch)
    kResetKernel();
    std::memset(kRam, 0, 0x500000);
    registerGuest(0x0F000110, (void*)&t_prio_body);
    makeTh(0x400110, 0x0F000110, 0x400, 5);   // B: lower priority (bigger number)
    CHECK(guest::CreateThread(0x400110) == 2);
    CHECK(guest::StartThread(2, 0x400310) == 0);
    kPumpToIdle();
    makeTh(0x400114, 0x0F000110, 0x400, 3);   // C: higher priority
    CHECK(guest::CreateThread(0x400114) == 3);
    CHECK(guest::StartThread(3, 0x400320) == 0);
    kPumpToIdle();
    CHECK(rd32(0x400310) != 0 && rd32(0x400320) != 0);  // both logged pre-sleep
    CHECK(guest::WakeupThread(2) == 0);
    CHECK(guest::WakeupThread(3) == 0);
    kPumpToIdle();
    CHECK(rd32(0x400320 + 4) < rd32(0x400310 + 4));  // prio 3 ran before prio 5

    // 4) iWakeupThread is host-served now (stub mirror): its body routes
    //    through the syscall dispatcher to the kernel's direct ready-queue
    //    wake (oracle Kernel/Syscalls/Thread.cpp). The game's ring-copy gen
    //    TU is excluded from compilation, so the ring RAM must stay
    //    untouched — mirroring the oracle, where the ring copy never runs.
    kResetKernel();
    std::memset(kRam, 0, 0x500000);
    wr32(0x2e98f0, 1);                        // libkernel-init gate != 0
    wr32(0x400130 + 0x04, 1); wr32(0x400130 + 0x08, 0);
    const int32_t semaR = guest::CreateSema(0x400130);
    wr32(0x1776570, (uint32_t)semaR);
    wr32(0x177657c, 0);
    wr32(0x1776580, 0x5a5a5a5au);             // poison: host path must not write it
    registerGuest(0x0F000120, (void*)&t_ring_body);
    makeTh(0x400110, 0x0F000120, 0x400, 2);
    CHECK(guest::CreateThread(0x400110) == 2);
    CHECK(guest::StartThread(2, 0) == 0);
    kPumpToIdle();
    CHECK(rd32(0x1776580) == 0x5a5a5a5au);    // ring slot untouched (copy excluded)
    CHECK(rd32(0x177657c) == 0);              // ring index never advanced
    CHECK(guest::PollSema((uint32_t)semaR) < 0);  // KE_SEMA_ZERO: no ring signal
    CHECK(kThreadCount() == 0);               // body ran + ExitDeleteThread

    // 5) ReferThreadStatus snapshot of a sleeping thread
    kResetKernel();
    std::memset(kRam, 0, 0x500000);
    registerGuest(0x0F000140, (void*)&t_sleep_body);
    makeTh(0x400110, 0x0F000140, 0x400, 4);
    CHECK(guest::CreateThread(0x400110) == 2);
    CHECK(guest::StartThread(2, 0) == 0);
    kPumpToIdle();
    CHECK(guest::ReferThreadStatus(2, 0x400800) == 0);
    CHECK(rd32(0x400800) == 4);               // THS_WAIT
    CHECK(rd32(0x400804) == 0x0F000140);      // func
    CHECK(rd32(0x40080C) == 0x400);           // stack_size
    CHECK(rd32(0x400814) == 4);               // initial priority
    CHECK(rd32(0x400824) == 1);               // waitType TSW_SLEEP
    CHECK(rd32(0x400828) == 0);               // waitId
    // Wake via the game wrapper's else branch: _iWakeupThread must receive
    // the target id (the W12 bare-call $4-forwarding fix at 00285ed8).
    registerGuest(0x0F000150, (void*)&t_wake_other_body);
    makeTh(0x400118, 0x0F000150, 0x400, 3);
    CHECK(guest::CreateThread(0x400118) == 3);
    CHECK(guest::StartThread(3, 2) == 0);
    kPumpToIdle();
    CHECK(rd32(0x400900) == 1);               // thread 2 woke and logged
    CHECK(kThreadCount() == 0);

    // 6) SetVSyncFlag one-shot + vblank tick
    kResetKernel();
    std::memset(kRam, 0, 0x500000);
    CHECK(guest::SetVSyncFlag(0x400600, 0x400608) == 0);
    CHECK(rd32(0x400600) == 0 && rd64(0x400608) == 0);
    kVblank();
    CHECK(rd32(0x400600) == 1);
    CHECK(rd64(0x400608) == 1);
    wr32(0x400600, 0);
    kVblank();                                // one-shot: no second write
    CHECK(rd32(0x400600) == 0);
    CHECK(rd64(0x400608) == 1);

    // 7) alarm through SetAlarm stub → vblank pump → handler invocation
    kResetKernel();
    std::memset(kRam, 0, 0x500000);
    registerGuest(0x0F000200, (void*)&alarm_handler);
    CHECK(guest::SetAlarm(10, 0x0F000200, 0x77) == 1);
    CHECK(guest::SetAlarm(1, 0x0F00FF00, 0) == -1);  // unregistered handler
    kVblank();                                // 16667 us > 10*64 us
    kPumpToIdle();
    CHECK(rd32(0x400504) == 1);               // $4 = alarm id
    CHECK(rd32(0x400508) == 10);              // $5 = ticks
    CHECK(rd32(0x40050C) == 0x77);            // $6 = argument
    CHECK(rd32(0x400500) == 0);               // no owner thread while idle
    // with a woken thread ready, the handler runs with that owner current
    registerGuest(0x0F000140, (void*)&t_sleep_body);
    makeTh(0x400110, 0x0F000140, 0x400, 10);
    CHECK(guest::CreateThread(0x400110) == 2);
    CHECK(guest::StartThread(2, 0) == 0);
    kPumpToIdle();                            // parks in SleepThread
    CHECK(guest::SetAlarm(1, 0x0F000200, 0) == 2);
    CHECK(guest::WakeupThread(2) == 0);       // ready but not yet pumped
    kVblank();
    kPumpToIdle();
    CHECK(rd32(0x400500) == 2);               // handler saw the owner tid
    CHECK(rd32(0x400900) == 1);               // owner then ran

    // 8) intc cause gate + handler chain (AddIntcHandler via dispatcher)
    kResetKernel();
    std::memset(kRam, 0, 0x500000);
    registerGuest(0x0F000300, (void*)&intc_handler);
    CHECK(guest::syscall(0x10, 0, 0x0F000300, 0, 0x55) == 1);  // AddIntcHandler
    CHECK(guest::syscall(0x15, 0) == 0);      // DisableIntc(cause 0)
    kVblank();
    kPumpToIdle();
    CHECK(rd32(0x400700) == 0 && rd32(0x400704) == 0);  // masked: no dispatch
    CHECK(guest::syscall(0x14, 0) == 0);      // EnableIntc(cause 0)
    kVblank();
    kPumpToIdle();
    CHECK(rd32(0x400700) == 0);               // $4 = cause
    CHECK(rd32(0x400704) == 0x55);            // $5 = argument

    // 9) Copy (syscallMemInstall) moves bytes
    for (uint32_t i = 0; i < 16; ++i) *(uint8_t*)G(0x402000 + i) = (uint8_t)(i + 1);
    CHECK(guest::syscallMemInstall(0x401000, 0x402000, 16) == 0);
    CHECK(std::memcmp(G(0x401000), G(0x402000), 16) == 0);

    // 10) boot main thread id 1 + vblank-paced pump
    kResetKernel();
    std::memset(kRam, 0, 0x500000);
    registerGuest(0x0F000400, (void*)&t_boot_body);
    kBootMainThread(0x0F000400);
    kPumpToIdle();
    CHECK(rd32(0x400A00) == 1);               // GetThreadId() == boot tid 1
    CHECK(guest::WakeupThread(1) == 0);
    kPumpToIdle();
    CHECK(rd32(0x400A04) == 1);               // resumed past the first sleep
    kPumpToIdle();                            // parked again: no crash

    // 11) WaitVSyncTick park (sceGsSyncV host path): the parked green thread
    // resumes inside kWaitVSync at kVblank and returns its fixedResult.
    kResetKernel();
    registerGuest(0x0F000B00, (void*)&t_vsync_body);
    makeTh(0x400B10, 0x0F000B00, 0x2000, 20);
    CHECK(guest::CreateThread(0x400B10) == 2);
    CHECK(guest::StartThread(2, 0) == 0);
    kPumpToIdle();
    CHECK(rd32(0x400B00) == 0);                 // parked: no vblank yet
    kVblank();
    kPumpToIdle();
    CHECK(rd32(0x400B00) == 7u);                // resumed with fixedResult 7
    CHECK(rd32(0x400B04) == 0);                 // parked again
    kVblank();
    kPumpToIdle();
    CHECK(rd32(0x400B04) == 0xFFFFFFFFu);       // fixedResult -1
    CHECK(kThreadCount() == 0);

    std::printf("test_w12_kernel: %s (%d failures)\n", fails ? "FAILED" : "ok", fails);
    return fails ? 1 : 0;
}
