#pragma once

// Minimal AArch64 context used by the single-executor guest kernel.  It is not
// a signal context: green threads only switch at explicit kernel boundaries.

#include <cstddef>
#include <cstdint>

#if !defined(__aarch64__)
#error "RT_GUEST_UCONTEXT_SHIM requires AArch64"
#endif

namespace rtguest::ucontext_shim {

struct Context {
    struct Stack {
        void* ss_sp;
        std::size_t ss_size;
    } uc_stack;
    Context* uc_link;
    uint64_t uc_sigmask;
    uint64_t x19_x28[10];
    uint64_t x29;
    uint64_t x30;
    uint64_t sp;
    uint64_t d8_d15[8];
    uint32_t fpcr;
    uint32_t reserved;
    void (*entry)(int);
    int argc;
    uint32_t reserved2;
    uintptr_t arg;
};

static_assert(offsetof(Context, x19_x28) == 32);
static_assert(offsetof(Context, x29) == 112);
static_assert(offsetof(Context, x30) == 120);
static_assert(offsetof(Context, sp) == 128);
static_assert(offsetof(Context, d8_d15) == 136);
static_assert(offsetof(Context, fpcr) == 200);
static_assert(offsetof(Context, entry) == 208);
static_assert(offsetof(Context, arg) == 224);

int getcontext(Context* uc);
void makecontext(Context* uc, void (*fn)(), int argc, ...);
int swapcontext(Context* from, const Context* to);

inline int sigemptyset(uint64_t* set) {
    *set = 0;
    return 0;
}

}  // namespace rtguest::ucontext_shim
