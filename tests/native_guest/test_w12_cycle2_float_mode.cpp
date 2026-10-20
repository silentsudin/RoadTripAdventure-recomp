// Independent W12 cycle-2 pin.  main.cpp's static is not callable, so this
// repeats its exact platform instructions and verifies their observable EE mode.
#include <cmath>
#include <cstdint>
#include <cstdio>

#if defined(__x86_64__)
#include <xmmintrin.h>
#endif

static int fails = 0, checks = 0;
static void check(bool ok, const char* what, int line) {
    ++checks;
    if (!ok) { std::fprintf(stderr, "FAIL %s (line %d)\n", what, line); ++fails; }
}
#define CHECK(c) check((c), #c, __LINE__)

static void enterExactAppMode() {
#if defined(__aarch64__)
    uint64_t fpcr;
    __asm__ volatile("mrs %0, fpcr" : "=r"(fpcr));
    fpcr = (fpcr & ~(3ull << 22)) | (3ull << 22) | (1ull << 24);
    __asm__ volatile("msr fpcr, %0" : : "r"(fpcr));
#elif defined(__x86_64__)
    unsigned csr = _mm_getcsr();
    csr = (csr & ~0x6000u) | 0x6000u | 0x8040u;
    _mm_setcsr(csr);
#endif
}

int main() {
#if !defined(__aarch64__) && !defined(__x86_64__)
    std::puts("w12_cycle2_float_mode_test: unsupported architecture (0 checks)");
    return 0;
#else
#if defined(__aarch64__)
    uint64_t saved;
    __asm__ volatile("mrs %0, fpcr" : "=r"(saved));
    uint64_t ieee = saved & ~((3ull << 22) | (1ull << 24));
    __asm__ volatile("msr fpcr, %0" : : "r"(ieee));
#else
    const unsigned saved = _mm_getcsr();
    _mm_setcsr(saved & ~(0x6000u | 0x8040u));
#endif
    volatile float rounded = 1.9f;
    volatile float denormal = 1.0e-40f;
    volatile float one = 1.0f;
    const float beforeRound = std::nearbyintf(rounded);
    const float beforeDenormal = denormal * one;
    enterExactAppMode();
    const float afterRound = std::nearbyintf(rounded);
    const float afterDenormal = denormal * one;

    CHECK(beforeRound == 2.0f);
    CHECK(afterRound == 1.0f);
    CHECK(afterDenormal == 0.0f);
    // Apple arm64 can already flush scalar subnormals with FZ cleared.  Where
    // the host exposes an IEEE baseline, require the transition too.
    if (beforeDenormal != 0.0f)
        CHECK(beforeDenormal != afterDenormal);
#if defined(__aarch64__)
    uint64_t fpcr;
    __asm__ volatile("mrs %0, fpcr" : "=r"(fpcr));
    CHECK(((fpcr >> 22) & 3u) == 3u && (fpcr & (1ull << 24)) != 0);
    __asm__ volatile("msr fpcr, %0" : : "r"(saved));
#else
    const unsigned csr = _mm_getcsr();
    CHECK((csr & 0x6000u) == 0x6000u && (csr & 0x8040u) == 0x8040u);
    _mm_setcsr(saved);
#endif
    std::printf("w12_cycle2_float_mode_test: %d checks, %d fails\n", checks, fails);
    return fails ? 1 : 0;
#endif
}
