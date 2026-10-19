// W12 host EE libkernel: threads / semaphores / alarms / intc handlers / heap
// / Copy / SIF-DMA models, wired to the syscall-stub TUs in guest/gen.
// Semantics mirrored from the oracle ps2xRuntime EeScheduler + Syscalls (the
// oracle is the fidelity reference; it is NOT linked here — see the decomp
// hard rule). Determinism: ucontext green threads switch only at kernel-call
// boundaries; virtual time advances only at kVblank; no host threads, no
// wall-clock reads, map iteration is id-ordered (std::map, not unordered_map).
// macOS: ucontext.h gates its API behind this feature macro; must precede includes.
#define _XOPEN_SOURCE 700
#include "guest_kernel.h"

#include "guest_shim.h"

#include <ucontext.h>

#include <algorithm>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <map>
#include <vector>

namespace rtguest {
namespace {

// ---- KE_* codes and thread-status/wait-type bytes (oracle EeScheduler +
// Helpers/State.h, ps2sdk kernel.h) ----
constexpr int32_t KE_OK = 0, KE_ERROR = -1;
constexpr int32_t KE_ILLEGAL_PRIORITY = -403, KE_ILLEGAL_THID = -406;
constexpr int32_t KE_UNKNOWN_THID = -407, KE_UNKNOWN_SEMID = -408;
constexpr int32_t KE_DORMANT = -413, KE_NOT_DORMANT = -414;
constexpr int32_t KE_NOT_SUSPEND = -415, KE_NOT_WAIT = -416;
constexpr int32_t KE_RELEASE_WAIT = -418, KE_SEMA_ZERO = -419;
constexpr int32_t KE_SEMA_OVF = -420, KE_WAIT_DELETE = -425;
constexpr uint32_t THS_RUN = 1, THS_READY = 2, THS_WAIT = 4, THS_SUSPEND = 8,
                   THS_WAITSUSPEND = 0xC, THS_DORMANT = 0x10;
constexpr uint32_t TSW_NONE = 0, TSW_SLEEP = 1, TSW_SEMA = 2;

constexpr int kPriorityCount = 128;
constexpr int kMainThreadId = 1, kFirstThreadId = 2, kLastThreadId = 255;
constexpr uint64_t kVBlankPeriodUs = 16667;  // oracle kVBlankPeriod
constexpr uint64_t kAlarmTickUs = 64;        // oracle kAlarmTickMicroseconds
constexpr uint32_t kDefaultHeapEnd = 0x01F00000u;  // oracle kDefaultGuestHeapEnd
constexpr uint32_t kSyscallMirrorPhys = 0x11F80u;  // 0x80011F80 & 0x1FFFFFFF
constexpr uint32_t kMinHostStack = 0x10000u;  // shortcut: host C frames are
                                              // wider than the guest's; the
                                              // declared guest stack region
                                              // stays free of our machine
                                              // stack (EE-stack fidelity at
                                              // W13 if hashes demand it)

enum Status : uint8_t { StReady, StRunning, StWaiting, StWaitingSuspended,
                        StSuspended, StDormant };
enum WaitReason : uint8_t { WaitNone, WaitSleep, WaitSemaphore, WaitVSync };

struct KThread {
    int id = 0, status = StDormant, initPri = 0, curPri = 0;
    int suspCount = 0, wakeCount = 0;
    uint32_t entry = 0, stack = 0, stackSize = 0, attr = 0, option = 0, arg = 0, gp = 0;
    WaitReason waitReason = WaitNone;
    int waitId = 0;
    int32_t resumeResult = 0;
    bool ownsStack = false;
    ucontext_t uc{};
    bool ucReady = false;
    std::vector<uint8_t> hostStack;
};

struct KSema {
    int id = 0, count = 0, maxCount = 0, initCount = 0;
    uint32_t attr = 0, option = 0;
    std::deque<int> waiters;
};

struct KAlarm {
    int id = 0;
    uint32_t ticks = 0, handler = 0, argument = 0, gp = 0;
    uint64_t deadlineUs = 0;
};

struct KIrq {
    int id = 0;
    uint32_t cause = 0, handler = 0, argument = 0, gp = 0;
    bool enabled = true;
    int order = 0;
};

struct Invocation {
    uint32_t pc = 0, a4 = 0, a5 = 0, a6 = 0;
};

// ---- state (single executor: the host thread driving the frame loop) ----
std::map<int, KThread> g_threads;
std::deque<int> g_ready[kPriorityCount];
int g_cur = 0, g_nextTid = kFirstThreadId;
std::map<int, KSema> g_sem;
int g_nextSemaId = 1;
std::map<int, KAlarm> g_alarm;
int g_nextAlarmId = 1;
std::map<int, KIrq> g_intc, g_dmac;
int g_nextIrqId = 1, g_intcHead = 0, g_intcTail = 1000, g_dmacHead = 0,
    g_dmacTail = 1000;
uint32_t g_intcMask = 0, g_dmacMask = 0;
std::deque<Invocation> g_invoc;
std::vector<int> g_retired;  // erase-after-swap (can't free a running stack)
ucontext_t g_pump{};
bool g_inInterrupt = false, g_resched = false;
uint64_t g_micro = 0, g_vsyncTick = 0;
uint32_t g_vsyncFlagAddr = 0, g_vsyncTickAddr = 0;
std::vector<int> g_vsyncWaiters;  // WaitVSyncTick parks (Interrupt.cpp:78-82)
uint32_t g_heapBase = 0, g_heapLimit = 0, g_heapBump = 0;
uint32_t g_gsImr = 0, g_gsCrt[5] = {};
uint64_t g_sifTransferId = 1;
std::map<uint32_t, uint32_t> g_sifRegs;
std::map<uint32_t, std::pair<uint32_t, uint32_t>> g_sifCmdHandlers;  // cid -> {fn, arg} (oracle g_sifCmdHandlers)
std::map<uint32_t, uint32_t> g_syscallOverride;  // num -> guest handler pc

struct Install { uint32_t dst, src, len; };  // Copy-tracked code blobs
std::vector<Install> g_installs;

// ---- little helpers ----
void wrU32(uint32_t a, uint32_t v) {
    if (a == 0) return;  // oracle writeGuestU32 guards 0
    std::memcpy(G(a), &v, 4);
}
void wrU64(uint32_t a, uint64_t v) {
    if (a == 0) return;
    std::memcpy(G(a), &v, 8);
}
uint32_t rdU32(uint32_t a) { uint32_t v = 0; std::memcpy(&v, G(a), 4); return v; }

KThread* thread(int id) {
    auto it = g_threads.find(id);
    return it == g_threads.end() ? nullptr : &it->second;
}
KThread* curThread() { return g_cur ? thread(g_cur) : nullptr; }

int allocatePositiveId(int& next, const auto& map_) {
    if (next <= 0) next = 1;
    while (map_.find(next) != map_.end()) ++next;  // ids never wrap (bounded
    return next++;                                 // by game call volume)
}

int allocateThreadId() {
    for (int attempts = 0; attempts <= kLastThreadId - kFirstThreadId; ++attempts) {
        const int cand = g_nextTid;
        g_nextTid = cand == kLastThreadId ? kFirstThreadId : cand + 1;
        if (!g_threads.count(cand)) return cand;
    }
    return 0;
}

// ---- ready-queue machinery (oracle enqueueReady/selectReady/…) ----
void enqueueReady(KThread& t, bool front = false) {
    t.status = StReady;
    if (t.curPri < 0 || t.curPri >= kPriorityCount) t.curPri = kPriorityCount - 1;
    if (front) g_ready[t.curPri].push_front(t.id);
    else g_ready[t.curPri].push_back(t.id);
}
void removeReady(KThread& t) {
    if (t.status != StReady) return;
    auto& q = g_ready[t.curPri];
    for (size_t i = 0; i < q.size(); ++i)
        if (q[i] == t.id) { q.erase(q.begin() + (long)i); break; }
}
KThread* selectReady() {
    for (auto& q : g_ready) {
        if (q.empty()) continue;
        const int id = q.front();
        q.pop_front();
        KThread* t = thread(id);
        return t;  // null only if a stale id slipped in; queues stay exact
    }
    return nullptr;
}
KThread* peekReadyFront() {
    for (auto& q : g_ready) {
        if (q.empty()) continue;
        return thread(q.front());
    }
    return nullptr;
}
void requestPreemptionIfHigher(const KThread& readyT) {
    const KThread* running = curThread();
    if (!running || readyT.curPri >= running->curPri) return;
    g_resched = true;
}
void makeReady(KThread& t, int32_t result) {
    t.waitReason = WaitNone; t.waitId = 0;
    t.resumeResult = result;
    if (t.suspCount != 0) { t.status = StSuspended; return; }
    enqueueReady(t);
    requestPreemptionIfHigher(t);
}
void makeDormant(KThread& t) {
    removeReady(t);
    if (t.waitReason == WaitSemaphore) {
        auto it = g_sem.find(t.waitId);
        if (it != g_sem.end()) {
            auto& w = it->second.waiters;
            for (size_t i = 0; i < w.size(); ++i)
                if (w[i] == t.id) { w.erase(w.begin() + (long)i); break; }
        }
    }
    t.waitReason = WaitNone; t.waitId = 0;
    t.status = StDormant; t.suspCount = 0; t.wakeCount = 0;
}

// Block the running green thread; returns when resumed (resumeResult).
// shortcut: called from an invocation (handler on the pump stack) this would
// swap from the wrong context — blocking in handler context is guest UB; the
// dispatcher refuses it (KE_ERROR) before ever calling here.
int32_t blockCurrent(WaitReason reason, int waitId) {
    KThread* self = curThread();
    self->waitReason = reason; self->waitId = waitId;
    self->status = self->suspCount == 0 ? StWaiting : StWaitingSuspended;
    g_cur = 0;
    g_resched = false;  // we leave the CPU anyway; the pump re-decides
    swapcontext(&self->uc, &g_pump);
    const int32_t r = self->resumeResult;
    self->resumeResult = 0;
    return r;
}

// Mirror of oracle transferIfRequested: the syscall that made a higher thread
// ready parks the caller (re-enqueued at the head) and lets the pump choose.
void transferIfRequested(bool interruptSafe) {
    if (interruptSafe || g_inInterrupt || !g_resched) return;
    KThread* self = curThread();
    if (!self) { g_resched = false; return; }
    enqueueReady(*self, /*front=*/true);
    g_cur = 0;
    g_resched = false;
    swapcontext(&self->uc, &g_pump);
}

void kExitCurrent(bool deleteRecord) {
    KThread* self = curThread();
    const int id = self->id;
    makeDormant(*self);
    g_cur = 0;
    if (deleteRecord && id != kMainThreadId) g_retired.push_back(id);
    swapcontext(&self->uc, &g_pump);
}

void thunk(int id) {
    KThread* t = thread(id);
    g_cur = t->id;
    rtCall(t->entry, t->arg);
    // A returning thread entry is undefined in libkernel (games exit
    // explicitly); retire like ExitDeleteThread rather than zombie the slot.
    kExitCurrent(true);
}

// ---- IRQ (intc/dmac handler chains, oracle addIrqHandler/dispatchIrq) ----
int addIrqHandler(bool dmac, uint32_t cause, uint32_t handler, bool append,
                  uint32_t argument, uint32_t gp) {
    auto& m = dmac ? g_dmac : g_intc;
    const int id = allocatePositiveId(g_nextIrqId, m);
    if (id == 0) return KE_ERROR;
    // intc Add goes to the head (negative order), Add…2/append to the tail —
    // exactly the oracle's head--/++tail scheme (shared counter for both ids
    // is our deterministic deviation; order values keep the chain stable).
    const int order = append ? (dmac ? ++g_dmacTail : ++g_intcTail)
                             : (dmac ? --g_dmacHead : --g_intcHead);
    m.emplace(id, KIrq{id, cause, handler, argument, gp, true, order});
    return id;
}
void dispatchIrq(bool dmac, uint32_t cause) {
    const uint32_t mask = dmac ? g_dmacMask : g_intcMask;
    if (cause >= 32 || (mask & (1u << cause)) == 0) return;
    const auto& m = dmac ? g_dmac : g_intc;
    std::vector<KIrq> matching;
    for (const auto& [id, h] : m)
        if (h.enabled && h.cause == cause && h.handler != 0 && fnEntry(h.handler))
            matching.push_back(h);
    std::sort(matching.begin(), matching.end(),
              [](const KIrq& a, const KIrq& b) { return a.order < b.order; });
    for (const KIrq& h : matching)
        g_invoc.push_back(Invocation{h.handler, cause, h.argument, 0});
}

// ---- alarms ----
int setAlarm(uint32_t ticks, uint32_t handler, uint32_t argument, uint32_t gp) {
    if (handler == 0 || !fnEntry(handler)) return KE_ERROR;
    const int id = allocatePositiveId(g_nextAlarmId, g_alarm);
    if (id == 0) return KE_ERROR;
    const uint64_t tickCount = ticks == 0 ? 1 : ticks;
    g_alarm.emplace(id, KAlarm{id, ticks, handler, argument, gp,
                               g_micro + tickCount * kAlarmTickUs});
    return id;
}
void fireDueAlarms() {
    std::vector<KAlarm> due;
    for (auto it = g_alarm.begin(); it != g_alarm.end();) {
        if (it->second.deadlineUs > g_micro) { ++it; continue; }
        due.push_back(it->second);
        it = g_alarm.erase(it);
    }
    std::sort(due.begin(), due.end(), [](const KAlarm& a, const KAlarm& b) {
        return a.deadlineUs != b.deadlineUs ? a.deadlineUs < b.deadlineUs
                                            : a.id < b.id;
    });
    for (const KAlarm& a : due)
        g_invoc.push_back(Invocation{a.handler, (uint32_t)a.id, a.ticks, a.argument});
}

// ---- heap (oracle SetupHeap/EndOfHeap + guestMalloc for stacks) ----
uint32_t heapAlloc(uint32_t bytes) {
    if (g_heapBump == 0) { g_heapBase = 0x01000000u; g_heapLimit = 0x01E00000u;
                           g_heapBump = g_heapBase; }  // default region stays
    bytes = (bytes + 15u) & ~15u;                       // below the frame arena
    if (g_heapBump + bytes > g_heapLimit) return 0;
    const uint32_t a = g_heapBump;
    g_heapBump += bytes;
    return a;
}

// ---- SifSetDma model: apply transfers now, fire dmac cause 5 (oracle) ----
int32_t sifSetDma(uint32_t listAddr, uint32_t count) {
    if (!listAddr) return 0;
    for (uint32_t i = 0; i < count; ++i) {
        const uint32_t desc = listAddr + i * 16;
        const uint32_t src = rdU32(desc), dst = rdU32(desc + 4);
        const uint32_t size = rdU32(desc + 8);
        if (!src || !dst || size == 0 || size > RT_GUEST_RAM_SIZE) return 0;
        std::memcpy(G(dst), G(src), size);
    }
    dispatchIrq(true, 5u);
    return (int32_t)g_sifTransferId++;
}

// Copy with blob tracking: a Copy that installs code (libkernel InitAlarm
// blobs) stays resolvable so setSyscall overrides pointing into the copy can
// be invoked through the source's registry entry (shortcut: source blobs are
// only invokable where their source range was ported and registered; the
// w12-boot task ports any blob the boot path actually executes).
void copyModel(uint32_t dst, uint32_t src, uint32_t size) {
    if (dst && src && size && size <= RT_GUEST_RAM_SIZE) {
        std::memcpy(G(dst), G(src), size);
        g_installs.push_back(Install{dst & 0x01FFFFFFu, src & 0x01FFFFFFu, size});
    }
}
uint32_t resolveOverridePc(uint32_t pc) {
    if (fnEntry(pc)) return pc;
    const uint32_t phys = pc & 0x01FFFFFFu;
    for (const Install& in : g_installs)
        if (phys >= in.dst && phys < in.dst + in.len) {
            const uint32_t src = in.src + (phys - in.dst);
            if (fnEntry(src)) return src;
        }
    return 0;
}

// ---- syscall operations (mirrors; intr = interruptSafe variant keys) ----
int32_t opCreateThread(uint32_t thAddr) {
    if (!thAddr) return KE_ERROR;
    uint8_t* p = (uint8_t*)G(thAddr);
    const uint32_t func = rdU32(thAddr + 0x04), stack = rdU32(thAddr + 0x08);
    const int32_t stackSize = (int32_t)rdU32(thAddr + 0x0C);
    const uint32_t gp = rdU32(thAddr + 0x10);
    const int32_t initPri = (int32_t)rdU32(thAddr + 0x14);
    const uint32_t attr = rdU32(thAddr + 0x1C), option = rdU32(thAddr + 0x20);
    if (stackSize < 0) return KE_ERROR;
    if (initPri < 1 || initPri >= kPriorityCount) return KE_ILLEGAL_PRIORITY;
    const int id = allocateThreadId();
    if (id == 0) return KE_ERROR;
    KThread t{};
    t.id = id; t.status = StDormant; t.entry = func; t.stack = stack;
    t.stackSize = (uint32_t)stackSize; t.gp = gp; t.attr = attr; t.option = option;
    t.initPri = t.curPri = initPri;
    g_threads.emplace(id, std::move(t));
    return id;
}

int32_t opStartThread(int id, uint32_t arg, bool intr) {
    KThread* t = thread(id);
    if (!t) return KE_UNKNOWN_THID;
    if (t->status != StDormant) return KE_NOT_DORMANT;
    if (!fnEntry(t->entry)) return KE_ERROR;
    if (t->stack == 0 && t->stackSize != 0) {
        t->stack = heapAlloc(t->stackSize);
        if (t->stack == 0) return KE_ERROR;
        t->ownsStack = true;  // shortcut: bump heap, never freed
    }
    const uint32_t bytes =
        (t->stackSize < kMinHostStack ? kMinHostStack : t->stackSize);
    t->hostStack.assign(bytes, 0);
    t->arg = arg;
    t->suspCount = 0; t->wakeCount = 0;
    t->waitReason = WaitNone; t->waitId = 0;
    getcontext(&t->uc);
    t->uc.uc_stack.ss_sp = t->hostStack.data();
    t->uc.uc_stack.ss_size = t->hostStack.size();
    t->uc.uc_link = nullptr;
    sigemptyset(&t->uc.uc_sigmask);
    makecontext(&t->uc, (void (*)())&thunk, 1, t->id);
    t->ucReady = true;
    enqueueReady(*t);
    requestPreemptionIfHigher(*t);
    transferIfRequested(intr);
    return KE_OK;
}

int32_t opDeleteThread(int id) {
    if (id <= kMainThreadId) return KE_ILLEGAL_THID;
    auto it = g_threads.find(id);
    if (it == g_threads.end()) return KE_UNKNOWN_THID;
    if (it->second.status != StDormant) return KE_NOT_DORMANT;
    g_threads.erase(it);
    return KE_OK;
}

int32_t opTerminateThread(int id) {
    if (id == 0 || id == g_cur) return KE_ILLEGAL_THID;
    KThread* t = thread(id);
    if (!t) return KE_UNKNOWN_THID;
    if (t->status == StDormant) return KE_DORMANT;
    makeDormant(*t);
    return KE_OK;
}

int32_t opSuspendThread(int id, bool intr) {
    if (id == 0) id = g_cur;
    KThread* t = thread(id);
    if (!t) return KE_UNKNOWN_THID;
    if (t->status == StDormant) return KE_DORMANT;
    ++t->suspCount;
    bool selfSwitch = false;
    switch (t->status) {
    case StRunning:
        t->status = StSuspended;
        if (id == g_cur) { g_cur = 0; selfSwitch = true; }
        g_resched = true;
        break;
    case StReady:
        removeReady(*t); t->status = StSuspended; break;
    case StWaiting: t->status = StWaitingSuspended; break;
    default: break;
    }
    if (intr && g_inInterrupt) g_resched = true;
    if (selfSwitch) {  // suspended self: park without re-enqueueing
        g_resched = false;
        swapcontext(&t->uc, &g_pump);
    }
    return KE_OK;
}

int32_t opResumeThread(int id, bool intr) {
    if (id == 0) return KE_ILLEGAL_THID;
    KThread* t = thread(id);
    if (!t) return KE_UNKNOWN_THID;
    if (t->suspCount == 0) return KE_NOT_SUSPEND;
    if (--t->suspCount != 0) return KE_OK;
    if (t->status == StWaitingSuspended) t->status = StWaiting;
    else if (t->status == StSuspended) {
        enqueueReady(*t);
        requestPreemptionIfHigher(*t);
        transferIfRequested(intr);
    }
    return KE_OK;
}

int32_t opSleepThread() {
    if (!g_cur || g_inInterrupt) return KE_ERROR;  // no thread / handler ctx = UB
    KThread* self = curThread();
    if (self->wakeCount != 0) { --self->wakeCount; return KE_OK; }
    return blockCurrent(WaitSleep, 0);
}

int32_t opWakeupThread(int id, bool intr) {
    if (id == 0 || id == g_cur) return KE_ILLEGAL_THID;
    KThread* t = thread(id);
    if (!t) return KE_UNKNOWN_THID;
    if (t->status == StDormant) return KE_DORMANT;
    if ((t->status == StWaiting || t->status == StWaitingSuspended) &&
        t->waitReason == WaitSleep)
        makeReady(*t, KE_OK);
    else
        ++t->wakeCount;
    (void)intr;
    return KE_OK;
}

int32_t opCancelWakeup(int id) {
    if (id == 0) id = g_cur;
    KThread* t = thread(id);
    if (!t) return KE_UNKNOWN_THID;
    const int old = t->wakeCount;
    t->wakeCount = 0;
    return old;
}

int32_t opChangePriority(int id, int prio, bool intr) {
    if (prio < 1 || prio >= kPriorityCount) return KE_ILLEGAL_PRIORITY;
    if (id == 0) id = g_cur;
    KThread* t = thread(id);
    if (!t) return KE_UNKNOWN_THID;
    if (t->status == StReady) {
        removeReady(*t);
        t->curPri = prio;
        enqueueReady(*t);
        requestPreemptionIfHigher(*t);
    } else {
        t->curPri = prio;
        if (t->status == StRunning)
            for (int p = 0; p < t->curPri; ++p)
                if (!g_ready[p].empty()) { g_resched = true; break; }
    }
    transferIfRequested(intr);
    return KE_OK;
}

int32_t opRotateReadyQueue(int prio, bool intr) {
    if (prio == 0) prio = curThread() ? curThread()->curPri : 0;
    if (prio < 0 || prio >= kPriorityCount) return KE_ILLEGAL_PRIORITY;
    KThread* self = curThread();
    if (self && self->curPri == prio) {
        enqueueReady(*self, /*front=*/false);
        g_cur = 0;
        g_resched = false;
        swapcontext(&self->uc, &g_pump);
    } else {
        auto& q = g_ready[prio];
        if (q.size() > 1) { const int head = q.front(); q.pop_front(); q.push_back(head); }
    }
    (void)intr;
    return KE_OK;
}

int32_t opReleaseWait(int id, bool intr) {
    if (id == 0) return KE_ILLEGAL_THID;
    KThread* t = thread(id);
    if (!t) return KE_UNKNOWN_THID;
    if (t->status != StWaiting && t->status != StWaitingSuspended) return KE_NOT_WAIT;
    if (t->waitReason == WaitSemaphore) {  // oracle removeFromWaitObject
        auto it = g_sem.find(t->waitId);
        if (it != g_sem.end()) {
            auto& w = it->second.waiters;
            for (size_t i = 0; i < w.size(); ++i)
                if (w[i] == t->id) { w.erase(w.begin() + (long)i); break; }
        }
    }
    t->waitReason = WaitNone; t->waitId = 0;
    makeReady(*t, KE_RELEASE_WAIT);
    (void)intr;
    return KE_OK;
}

int32_t opReferThreadStatus(int id, uint32_t ptr) {
    if (id == 0) id = g_cur;
    KThread* t = thread(id);
    if (!t) return KE_UNKNOWN_THID;
    if (!ptr) return KE_ERROR;
    uint8_t* out = (uint8_t*)G(ptr);
    const uint32_t zero = 0;
    uint32_t s;
    switch (t->status) {
    case StRunning: s = THS_RUN; break;
    case StReady: s = THS_READY; break;
    case StWaiting: s = THS_WAIT; break;
    case StWaitingSuspended: s = THS_WAITSUSPEND; break;
    case StSuspended: s = THS_SUSPEND; break;
    default: s = THS_DORMANT; break;
    }
    auto put = [&](int off, uint32_t v) { std::memcpy(out + off, &v, 4); };
    for (int off = 0; off < 0x30; off += 4) put(off, zero);
    put(0x00, s);
    put(0x04, t->entry);
    put(0x08, t->stack);
    put(0x0C, t->stackSize);
    put(0x10, t->gp);
    put(0x14, (uint32_t)t->initPri);
    put(0x18, (uint32_t)t->curPri);
    put(0x1C, t->attr);
    put(0x20, t->option);
    put(0x24, t->waitReason == WaitSleep ? TSW_SLEEP
               : t->waitReason == WaitSemaphore ? TSW_SEMA : TSW_NONE);
    put(0x28, (uint32_t)t->waitId);
    put(0x2C, (uint32_t)t->wakeCount);
    return KE_OK;
}

// ---- semaphores (ee_sema_t: count@0 max@4 init@8 waiters@12 attr@16
// option@20, oracle CreateSema reads init=+8 max=+4 attr=+16 option=+20) ----
int32_t opCreateSema(uint32_t thAddr) {
    if (!thAddr) return KE_ERROR;
    const int32_t maxCount = (int32_t)rdU32(thAddr + 0x04);
    const int32_t initCount = (int32_t)rdU32(thAddr + 0x08);
    const uint32_t attr = rdU32(thAddr + 0x10), option = rdU32(thAddr + 0x14);
    if (maxCount <= 0 || initCount < 0 || initCount > maxCount) return KE_ERROR;
    const int id = allocatePositiveId(g_nextSemaId, g_sem);
    if (id == 0) return KE_ERROR;
    g_sem.emplace(id, KSema{id, initCount, maxCount, initCount, attr, option, {}});
    return id;
}
int32_t opDeleteSema(int id, bool intr) {
    auto it = g_sem.find(id);
    if (it == g_sem.end()) return KE_UNKNOWN_SEMID;
    std::deque<int> waiters = std::move(it->second.waiters);
    g_sem.erase(it);
    for (const int tid : waiters)
        if (KThread* w = thread(tid)) makeReady(*w, KE_WAIT_DELETE);
    transferIfRequested(intr);  // oracle deleteSemaphoreImpl
    return id;
}
int32_t opSignalSema(int id, bool intr) {
    auto it = g_sem.find(id);
    if (it == g_sem.end()) return KE_UNKNOWN_SEMID;
    KSema& s = it->second;
    if (!s.waiters.empty()) {
        const int wid = s.waiters.front();
        s.waiters.pop_front();
        if (KThread* w = thread(wid)) makeReady(*w, id);  // wake result = id
        transferIfRequested(intr);  // oracle SignalSema wrapper
        return id;
    }
    if (s.count == s.maxCount) return KE_SEMA_OVF;
    ++s.count;
    return id;
}
int32_t opPollSema(int id, bool intr) {
    auto it = g_sem.find(id);
    if (it == g_sem.end()) return KE_UNKNOWN_SEMID;
    if (it->second.count == 0) return KE_SEMA_ZERO;
    --it->second.count;
    transferIfRequested(intr);  // oracle PollSema wrapper (usually a no-op)
    return id;
}
int32_t opWaitSema(int id, bool intr) {
    auto it = g_sem.find(id);
    if (it == g_sem.end()) return KE_UNKNOWN_SEMID;
    if (!g_cur || g_inInterrupt) return KE_ERROR;  // host/handler ctx: UB to block
    KSema& s = it->second;
    if (s.count != 0) {
        --s.count;
        transferIfRequested(intr);  // oracle waitSemaphoreImpl fast path
        return id;
    }
    s.waiters.push_back(g_cur);
    return blockCurrent(WaitSemaphore, id);
}

// ---- SetVSyncFlag / vblank ----
int32_t opSetVSyncFlag(uint32_t flagAddr, uint32_t tickAddr) {
    g_vsyncFlagAddr = flagAddr;
    g_vsyncTickAddr = tickAddr;
    wrU32(flagAddr, 0);
    wrU64(tickAddr, 0);
    return KE_OK;
}

// WaitVSyncTick park (Interrupt.cpp:78-82 → EeScheduler::waitVSync). The
// fixedResult is carried in waitId; kVblank resumes every parked thread with
// it (sceGsSyncV's per-GParam interlace choice lives in the GS host body).
int32_t opWaitVSync(int fixedResult) {
    if (!g_cur || g_inInterrupt) return KE_ERROR;  // host/handler ctx: no block
    g_vsyncWaiters.push_back(curThread()->id);
    return blockCurrent(WaitVSync, fixedResult);
}

void wakeVSyncWaiters() {
    std::vector<int> invs;
    invs.swap(g_vsyncWaiters);
    for (int id : invs) {
        KThread* t = thread(id);
        if (t && t->waitReason == WaitVSync) makeReady(*t, (int32_t)t->waitId);
    }
}

}  // namespace

// ---- frame-loop driver ----
void kVblank() {
    resetFrameArena();  // bump arena lives one vblank (shortcut, W13 stack model)
    g_micro += kVBlankPeriodUs;
    fireDueAlarms();  // shortcut: alarm-vs-vblank tie lands on the vblank
    ++g_vsyncTick;    // boundary (oracle uses cycle deadlines; W13 audit)
    if (g_vsyncFlagAddr != 0) wrU32(g_vsyncFlagAddr, 1);
    if (g_vsyncTickAddr != 0) wrU64(g_vsyncTickAddr, g_vsyncTick);
    g_vsyncFlagAddr = 0;  // one-shot like the oracle
    g_vsyncTickAddr = 0;
    dispatchIrq(false, 0u);  // intc cause 0 (vblank handlers)
    wakeVSyncWaiters();      // WaitVSyncTick parks (sceGsSyncV host body)
}

int32_t kWaitVSync(int fixedResult) { return opWaitVSync(fixedResult); }

void kPumpToIdle() {
    getcontext(&g_pump);
    for (;;) {
        for (const int id : g_retired) g_threads.erase(id);
        g_retired.clear();
        if (!g_invoc.empty()) {
            // Handlers run on the pump stack with the would-be-next thread as
            // "current": GetThreadId / self checks in the game's i* wrappers
            // then behave as in the oracle's invocation-on-owner model.
            KThread* owner = peekReadyFront();
            std::deque<Invocation> invs;
            invs.swap(g_invoc);
            g_inInterrupt = true;
            g_cur = owner ? owner->id : 0;
            for (const Invocation& iv : invs)
                if (iv.pc && fnEntry(iv.pc)) rtCall(iv.pc, iv.a4, iv.a5, iv.a6);
                else if (iv.pc) noteFault(iv.pc);
            g_cur = 0;
            g_inInterrupt = false;
            continue;
        }
        KThread* t = selectReady();
        if (!t) return;
        t->status = StRunning;
        g_cur = t->id;
        swapcontext(&g_pump, &t->uc);
    }
}

uint64_t kVirtualMicro() { return g_micro; }
int32_t kCurrentThreadId() { return g_cur; }
uint32_t kThreadCount() { return (uint32_t)g_threads.size(); }
uint32_t kSemaCount() { return (uint32_t)g_sem.size(); }

void kBootMainThread(uint32_t entry) {
    kResetKernel();  // boot starts from a fresh kernel incl. the SIF boot-reg seed
    g_threads.erase(kMainThreadId);
    KThread t{};
    t.id = kMainThreadId; t.entry = entry;
    t.initPri = t.curPri = 127;  // idle-class like the oracle main thread
    t.hostStack.assign(0x40000u, 0);
    auto it = g_threads.emplace(kMainThreadId, std::move(t)).first;
    KThread& m = it->second;
    getcontext(&m.uc);
    m.uc.uc_stack.ss_sp = m.hostStack.data();
    m.uc.uc_stack.ss_size = m.hostStack.size();
    m.uc.uc_link = nullptr;
    sigemptyset(&m.uc.uc_sigmask);
    makecontext(&m.uc, (void (*)())&thunk, 1, kMainThreadId);
    m.ucReady = true;
    enqueueReady(m);
}

void seedSifRegs() {
    // Oracle Stubs/SIF.cpp reset(): boot-status reg (0x4) comes up ready;
    // the game's loader spins on sceSifGetReg(4) & 0x20000 until this is set.
    g_sifRegs[0x4] = 0x00020000;  // kSifBootReadyMask
    g_sifRegs[0x80000000] = 0;    // main addr
    g_sifRegs[0x80000001] = 0;    // sub addr
    g_sifRegs[0x80000002] = 0;    // ms-com
}

void kResetKernel() {
    g_threads.clear(); g_retired.clear(); g_sem.clear(); g_alarm.clear();
    g_intc.clear(); g_dmac.clear(); g_invoc.clear(); g_installs.clear();
    g_vsyncWaiters.clear();
    g_sifRegs.clear(); g_syscallOverride.clear(); g_sifCmdHandlers.clear();
    seedSifRegs();
    for (auto& q : g_ready) q.clear();
    g_cur = 0; g_nextTid = kFirstThreadId; g_nextSemaId = 1; g_nextAlarmId = 1;
    g_nextIrqId = 1; g_intcHead = 0; g_intcTail = 1000; g_dmacHead = 0;
    g_dmacTail = 1000; g_intcMask = 0; g_dmacMask = 0; g_inInterrupt = false;
    g_resched = false; g_micro = 0; g_vsyncTick = 0; g_vsyncFlagAddr = 0;
    g_vsyncTickAddr = 0; g_heapBase = 0; g_heapLimit = 0; g_heapBump = 0;
    g_gsImr = 0; g_sifTransferId = 1;
}

// Host library services (guest_libs*) — mirrors how the oracle's
// ps2_stubs/ps2_syscalls reach EeScheduler ops (Kernel/Stubs/SIF.cpp,
// Syscalls/RPC.cpp).
int32_t kSignalSema(int id, bool intr) { return opSignalSema(id, intr); }
uint32_t kHeapAlloc(uint32_t bytes) { return heapAlloc(bytes); }
void kResetSifRegs() { seedSifRegs(); }
void kSetCmdHandler(uint32_t cid, uint32_t fn, uint32_t arg) {
    g_sifCmdHandlers[cid] = {fn, arg};
}
void kEraseCmdHandler(uint32_t cid) { g_sifCmdHandlers.erase(cid); }
void kFireDmacCause(uint32_t cause) { dispatchIrq(true, cause); }

}  // namespace rtguest

// The dispatcher below references the kernel's internal state/ops unqualified.
using namespace rtguest;

// ---- the dispatcher (guest::syscall, declared in guest_shim.h) ----
namespace guest {

int32_t syscall(uint64_t num, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3,
                uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7) {
    const uint32_t key = (uint32_t)num;
    const bool intr = (key & 0x80000000u) != 0;  // sign-extended = i-variant
    // Boot-bisection telemetry (order is deterministic: the pump is).
    static const bool ktrace = std::getenv("RT_KTRACE") != nullptr;
    if (ktrace)
        std::fprintf(stderr, "[ktrace] t=%d num=0x%x a0=0x%x a1=0x%x a2=0x%x a3=0x%x\n",
                     kCurrentThreadId(), key, a0, a1, a2, a3);

    // setSyscall override first, like the oracle dispatchSyscallOverride.
    auto ov = g_syscallOverride.find(key);
    if (ov != g_syscallOverride.end()) {
        const uint32_t pc = resolveOverridePc(ov->second);
        if (!pc) { noteFault(ov->second); return KE_ERROR; }
        return (int32_t)rtCall(pc, a0, a1, a2, a3, a4, a5, a6, a7);
    }

    switch (key) {
    // thread ops
    case 0x04: case 0x23:  // ExitThread (Ps2ExitThreadId / entry root)
        kExitCurrent(false);
        return KE_OK;  // unreachable
    case 0x24:  // ExitDeleteThread
        kExitCurrent(true);
        return KE_OK;  // unreachable
    case 0x20: return opCreateThread(a0);
    case 0x21: return opDeleteThread((int)a0);
    case 0x22: return opStartThread((int)a0, a1, intr);
    case 0x26: case 0xffffffda: return opTerminateThread((int)a0);
    case 0x29: case 0xffffffd6: return opChangePriority((int)a0, (int)a1, intr);
    case 0x2b: case 0xffffffd4: return opRotateReadyQueue((int)a0, intr);
    case 0x2d: case 0xffffffd2: return opReleaseWait((int)a0, intr);
    case 0x2f: case 0xffffffd1: return g_cur;  // GetThreadId
    case 0x30: case 0xffffffcf: return opReferThreadStatus((int)a0, a1);
    case 0x32: return opSleepThread();
    case 0x33: case 0xffffffcc: return opWakeupThread((int)a0, intr);
    case 0x35: case 0xffffffca: return opCancelWakeup((int)a0);
    case 0x37: return opSuspendThread((int)a0, false);
    case 0xffffffc8: return opSuspendThread((int)a0, true);
    // thread-system ops
    case 0x3c: {  // SetupThread(gp, stack, size) — boot thread fields;
        KThread* m = curThread() ? curThread() : thread(kMainThreadId);
        if (m) { m->gp = a0; m->stack = a1; m->stackSize = a2; }
        return (int32_t)((a1 + a2) & ~0xFu);  // returned sp; ours is the
    }                                         // green host stack (no-op use)
    case 0x3d: {  // SetupHeap(base, size): oracle model
        const uint32_t base = (a0 + 0xFu) & ~0xFu;
        uint64_t limit = kDefaultHeapEnd;
        if (a1 != 0 && a1 != 0xFFFFFFFFu) {
            const uint64_t cand = (uint64_t)base + a1;
            limit = cand < limit ? cand : limit;
        }
        if (limit <= base) limit = kDefaultHeapEnd;
        g_heapBase = base; g_heapLimit = (uint32_t)limit; g_heapBump = base;
        return (int32_t)base;
    }
    case 0x3e: return (int32_t)(g_heapLimit ? g_heapLimit : kDefaultHeapEnd);
    // sema ops (ee_sema_t reads inside the op)
    case 0x40: return opCreateSema(a0);
    case 0x41: case 0xffffffb7: return opDeleteSema((int)a0, intr);
    case 0x42: case 0xffffffbd: return opSignalSema((int)a0, intr);
    case 0x44: return opWaitSema((int)a0, intr);
    case 0x45: case 0xffffffba: return opPollSema((int)a0, intr);
    // alarms
    case 0xfc: case 0xffffff03: return setAlarm(a0, a1, a2, /*gp=*/0);
    case 0xfe: case 0xffffff01:
        return g_alarm.erase((int)a0) == 0 ? KE_ERROR : KE_OK;
    // intc / dmac handlers and cause enables
    case 0x10: return addIrqHandler(false, a0, a1, a2 != 0, a3, 0);
    case 0x11: {
        auto it = g_intc.find((int)a1);
        if (it != g_intc.end() && it->second.cause == a0) g_intc.erase(it);
        return KE_OK;
    }
    case 0x12: return addIrqHandler(true, a0, a1, a2 != 0, a3, 0);
    case 0x13: {
        auto it = g_dmac.find((int)a1);
        if (it != g_dmac.end() && it->second.cause == a0) g_dmac.erase(it);
        return KE_OK;
    }
    case 0x14: case 0xffffffe6: g_intcMask |= a0 < 32 ? 1u << a0 : 0; return KE_OK;
    case 0x15: case 0xffffffe5: g_intcMask &= a0 < 32 ? ~(1u << a0) : ~0u; return KE_OK;
    case 0x16: case 0xffffffe4: g_dmacMask |= a0 < 32 ? 1u << a0 : 0; return KE_OK;
    case 0x17: case 0xffffffe3: g_dmacMask &= a0 < 32 ? ~(1u << a0) : ~0u; return KE_OK;
    // GS-adjacent shadows (display path reads these at w12-boot)
    case 0x02:
        g_gsCrt[0] = a0; g_gsCrt[1] = a1; g_gsCrt[2] = a2; g_gsCrt[3] = a3;
        g_gsCrt[4] = a4;
        return KE_OK;
    case 0x70: case 0xffffff90: return (int32_t)g_gsImr;
    case 0x71: {
        const uint32_t old = g_gsImr; g_gsImr = a0; return (int32_t)old;
    }
    case 0xffffff8f: { const uint32_t old = g_gsImr; g_gsImr = a0; return (int32_t)old; }
    case 0x73: return opSetVSyncFlag(a0, a1);
    // SIF DMA models (oracle: transfers applied immediately)
    case 0x77: case 0xffffff89: return sifSetDma(a0, a1);
    case 0x76: case 0xffffff8a: return -1;  // DmaStat: done
    case 0x78: case 0xffffff88: return KE_OK;
    case 0x6b: return KE_OK;
    case 0x79: {
        uint32_t& slot = g_sifRegs[a0];
        const uint32_t prev = slot; slot = a1;
        return (int32_t)prev;
    }
    case 0x7a: return (int32_t)(g_sifRegs.count(a0) ? g_sifRegs[a0] : 0);
    // syscall table mirror (game InitThread/InitAlarm bookkeeping)
    case 0x5b: {  // GetEntryAddress
        uint32_t v = 0;
        if (a0 < 0x80000u / 4) std::memcpy(&v, G(kSyscallMirrorPhys + a0 * 4u), 4);
        return (int32_t)v;
    }
    case 0x74:  // SetSyscall(num, handler): store + mirror word
        g_syscallOverride[a0] = a1;
        if (a0 < 0x80000u / 4) {
            const uint32_t phys = kSyscallMirrorPhys + a0 * 4u;
            std::memcpy(G(phys), &a1, 4);
        }
        return KE_OK;
    case 0x5a: copyModel(a0, a1, a2); return KE_OK;
    case 0x64: case 0xffffff9c: return KE_OK;  // FlushCache: nothing cached
    case 0x7c: case 0xffffff84: return KE_OK;  // Deci2Call: release no-op
    default:
        std::fprintf(stderr, "[rtguest] unimplemented syscall key=0x%08x a0=0x%x\n",
                     key, a0);
        noteFault(0xE0000000u | key);
        return KE_OK;  // oracle bootstrap default
    }
}

}  // namespace guest
