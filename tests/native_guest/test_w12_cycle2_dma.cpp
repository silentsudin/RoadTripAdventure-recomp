// Independent W12 cycle-2 pin: DMA register state is IO state, never EE RAM.
#include "guest/guest_shim.h"

#include <cstdint>
#include <cstdio>
#include <cstring>

namespace guest {
uint32_t sceDmaPutStallAddr(uint32_t, ...);
uint32_t sceDmaSendN(uint32_t, uint32_t, uint32_t, ...);
uint32_t sceDmaSync(uint32_t, uint32_t, uint32_t, ...);
}

static int fails = 0, checks = 0;
static void check(bool ok, const char* what, int line) {
    ++checks;
    if (!ok) { std::fprintf(stderr, "FAIL %s (line %d)\n", what, line); ++fails; }
}
#define CHECK(c) check((c), #c, __LINE__)

static uint32_t word(uint32_t address) {
    uint32_t value = 0;
    std::memcpy(&value, rtguest::kRam + address, sizeof(value));
    return value;
}

static void putWord(uint32_t address, uint32_t value) {
    std::memcpy(rtguest::kRam + address, &value, sizeof(value));
}

int main() {
    // 0x10008000's 25-bit aliases are the old CHCR/MADR/QWC/TADR RAM leaks.
    constexpr uint32_t kMto0 = 0x10008000u;
    constexpr uint32_t kAliases[] = {0x00008000u, 0x00008010u, 0x00008020u,
                                     0x00008030u, 0x00008040u, 0x00008050u};
    constexpr uint32_t kSentinel[] = {0x9a000000u, 0x9a000001u, 0x9a000002u,
                                      0x9a000003u, 0x9a000004u, 0x9a000005u};
    for (unsigned i = 0; i < sizeof(kAliases) / sizeof(*kAliases); ++i)
        putWord(kAliases[i], kSentinel[i]);

    // Normal MTO0 submission writes CHCR/MADR/QWC/TADR and completion writes
    // CHCR again.  It is the public libdma path that formerly dirtied aliases.
    constexpr uint32_t payload = 0x00200000u;
    std::memset(rtguest::kRam + payload, 0, 16);
    CHECK(guest::sceDmaSendN(kMto0, payload, 16) == 0);
    CHECK(guest::sceDmaSync(kMto0, 0, 0) == 0);
    for (unsigned i = 0; i < sizeof(kAliases) / sizeof(*kAliases); ++i)
        CHECK(word(kAliases[i]) == kSentinel[i]);

    // STADR is a read-after-write public register path.  A full-width value
    // must round-trip from the DMA side table without touching its RAM alias.
    constexpr uint32_t kStadrAlias = 0x0000e060u;
    putWord(kStadrAlias, 0x12345678u);
    CHECK(guest::sceDmaPutStallAddr(0x89abcdefu) == 0u);
    CHECK(guest::sceDmaPutStallAddr(0xffffffffu) == 0x89abcdefu);
    CHECK(word(kStadrAlias) == 0x12345678u);

    std::printf("w12_cycle2_dma_test: %d checks, %d fails\n", checks, fails);
    return fails ? 1 : 0;
}
