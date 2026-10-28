#include "ucontext_shim.h"

#include <cstdarg>
#include <cstdlib>

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace {

using rtguest::ucontext_shim::Context;

#if defined(__aarch64__)

extern "C" [[noreturn]] void rt_guest_ucontext_invoke(Context* uc) {
    uc->entry(static_cast<int>(uc->arg));
    std::abort();
}

extern "C" [[noreturn, naked]] void rt_guest_ucontext_trampoline() {
#if defined(__APPLE__)
    __asm__ volatile(
        "mov x0, x19\n"
        "bl _rt_guest_ucontext_invoke\n"
        "brk #0\n");
#else
    __asm__ volatile(
        "mov x0, x19\n"
        "bl rt_guest_ucontext_invoke\n"
        "brk #0\n");
#endif
}

#elif defined(_WIN32)

constexpr std::size_t kWin32FiberStackFloor = 0x10000;

void WINAPI fiberStart(void* arg) {
    auto* uc = static_cast<Context*>(arg);
    uc->entry(static_cast<int>(uc->arg));
    std::abort();  // guest entries park on swapcontext and never return
}

#endif

}  // namespace

#if defined(__aarch64__)

int __attribute__((naked)) rtguest::ucontext_shim::getcontext(Context*) {
    __asm__ volatile(
        "stp x19, x20, [x0, #32]\n"
        "stp x21, x22, [x0, #48]\n"
        "stp x23, x24, [x0, #64]\n"
        "stp x25, x26, [x0, #80]\n"
        "stp x27, x28, [x0, #96]\n"
        "str x29, [x0, #112]\n"
        "str x30, [x0, #120]\n"
        "mov x1, sp\n"
        "str x1, [x0, #128]\n"
        "stp d8, d9, [x0, #136]\n"
        "stp d10, d11, [x0, #152]\n"
        "stp d12, d13, [x0, #168]\n"
        "stp d14, d15, [x0, #184]\n"
        "mrs x1, fpcr\n"
        "str w1, [x0, #200]\n"
        "mrs x1, fpsr\n"
        "str w1, [x0, #204]\n"
        "mov w0, wzr\n"
        "ret\n");
}

int __attribute__((naked)) rtguest::ucontext_shim::swapcontext(Context*, const Context*) {
    __asm__ volatile(
        "stp x19, x20, [x0, #32]\n"
        "stp x21, x22, [x0, #48]\n"
        "stp x23, x24, [x0, #64]\n"
        "stp x25, x26, [x0, #80]\n"
        "stp x27, x28, [x0, #96]\n"
        "str x29, [x0, #112]\n"
        "str x30, [x0, #120]\n"
        "mov x2, sp\n"
        "str x2, [x0, #128]\n"
        "stp d8, d9, [x0, #136]\n"
        "stp d10, d11, [x0, #152]\n"
        "stp d12, d13, [x0, #168]\n"
        "stp d14, d15, [x0, #184]\n"
        "mrs x2, fpcr\n"
        "str w2, [x0, #200]\n"
        "mrs x2, fpsr\n"
        "str w2, [x0, #204]\n"
        "ldp x19, x20, [x1, #32]\n"
        "ldp x21, x22, [x1, #48]\n"
        "ldp x23, x24, [x1, #64]\n"
        "ldp x25, x26, [x1, #80]\n"
        "ldp x27, x28, [x1, #96]\n"
        "ldr x29, [x1, #112]\n"
        "ldr x30, [x1, #120]\n"
        "ldp d8, d9, [x1, #136]\n"
        "ldp d10, d11, [x1, #152]\n"
        "ldp d12, d13, [x1, #168]\n"
        "ldp d14, d15, [x1, #184]\n"
        "ldr w2, [x1, #200]\n"
        "msr fpcr, x2\n"
        "ldr w2, [x1, #204]\n"
        "msr fpsr, x2\n"
        "ldr x2, [x1, #128]\n"
        "mov sp, x2\n"
        "mov w0, wzr\n"
        "br x30\n");
}

#elif defined(_WIN32)

// Fibers switch the nonvolatile registers and, with FLOAT_FLAG_SWITCH, the
// floating-point state (MXCSR included) — what the system ucontext saved.
int rtguest::ucontext_shim::getcontext(Context* uc) {
    (void)uc;  // the running fiber is recorded by swapcontext, not captured here
    return 0;
}

int rtguest::ucontext_shim::swapcontext(Context* from, const Context* to) {
    if (!IsThreadAFiber()) ConvertThreadToFiberEx(nullptr, FIBER_FLAG_FLOAT_SWITCH);
    from->fiber = GetCurrentFiber();
    Context* target = const_cast<Context*>(to);
    if (!target->fiber) std::abort();  // only makecontext'd contexts are targets
    SwitchToFiber(target->fiber);
    return 0;
}

#endif

#if defined(_WIN32)
void rtguest::ucontext_shim::releaseContext(Context& uc) {
    if (uc.fiber && uc.fiber != GetCurrentFiber()) DeleteFiber(uc.fiber);
    uc.fiber = nullptr;
}
#else
void rtguest::ucontext_shim::releaseContext(Context&) {}  // the asm switch owns no OS resource
#endif

void rtguest::ucontext_shim::makecontext(Context* uc, void (*fn)(), int argc, ...) {
    uintptr_t arg = 0;
    va_list args;
    va_start(args, argc);
    if (argc == 1) arg = static_cast<uintptr_t>(va_arg(args, int));
    va_end(args);
    if (argc > 1 || uc->uc_stack.ss_sp == nullptr || uc->uc_stack.ss_size == 0)
        std::abort();

#if defined(_WIN32)
    // A restarted thread gets a fresh fiber; the old one is parked at its exit.
    if (uc->fiber && uc->fiber != GetCurrentFiber()) DeleteFiber(uc->fiber);
    const SIZE_T stack = uc->uc_stack.ss_size < kWin32FiberStackFloor
                             ? kWin32FiberStackFloor
                             : static_cast<SIZE_T>(uc->uc_stack.ss_size);
    uc->fiber = CreateFiberEx(0, stack, FIBER_FLAG_FLOAT_SWITCH, &fiberStart, uc);
    if (!uc->fiber) std::abort();
#else
    const uintptr_t top = reinterpret_cast<uintptr_t>(uc->uc_stack.ss_sp) + uc->uc_stack.ss_size;
    uc->sp = top & ~uintptr_t(15);
    uc->x19_x28[0] = reinterpret_cast<uintptr_t>(uc);
    uc->x30 = reinterpret_cast<uintptr_t>(&rt_guest_ucontext_trampoline);
#endif
    uc->entry = reinterpret_cast<void (*)(int)>(fn);
    uc->argc = argc;
    uc->arg = arg;
}
