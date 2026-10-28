#include "ucontext_shim.h"

#include <cstdarg>
#include <cstdlib>

namespace {

using rtguest::ucontext_shim::Context;

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

}  // namespace

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
        "ldr x2, [x1, #128]\n"
        "mov sp, x2\n"
        "mov w0, wzr\n"
        "br x30\n");
}

void rtguest::ucontext_shim::makecontext(Context* uc, void (*fn)(), int argc, ...) {
    uintptr_t arg = 0;
    va_list args;
    va_start(args, argc);
    if (argc == 1) arg = static_cast<uintptr_t>(va_arg(args, int));
    va_end(args);
    if (argc > 1 || uc->uc_stack.ss_sp == nullptr || uc->uc_stack.ss_size == 0)
        std::abort();

    const uintptr_t top = reinterpret_cast<uintptr_t>(uc->uc_stack.ss_sp) + uc->uc_stack.ss_size;
    uc->sp = top & ~uintptr_t(15);
    uc->x19_x28[0] = reinterpret_cast<uintptr_t>(uc);
    uc->x30 = reinterpret_cast<uintptr_t>(&rt_guest_ucontext_trampoline);
    uc->entry = reinterpret_cast<void (*)(int)>(fn);
    uc->argc = argc;
    uc->arg = arg;
}
