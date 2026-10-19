// Pins for the authored __moddi3 (0x0028d8a8): signed 64-bit remainder per the
// libgcc pseudocode's externally visible semantics (sign of result follows the
// dividend; zero divisor break-traps — not exercised here because shim trap()
// is non-fatal). Values computed in exact integer arithmetic (Python ints).
#include "../../src/native/guest/guest_shim.h"

#include <cassert>
#include <cstdint>

namespace guest {
uint64_t __moddi3(uint64_t, uint64_t);
uint64_t __umoddi3(uint64_t, uint64_t, ...);
uint64_t __udivdi3(uint64_t, uint64_t, ...);
}

static uint64_t m(int64_t a, int64_t b) { return guest::__moddi3((uint64_t)a, (uint64_t)b); }

int main() {
  assert((int64_t)m(7, 3) == 1);
  assert((int64_t)m(-7, 3) == -1);   // dividend sign governs
  assert((int64_t)m(7, -3) == 1);
  assert((int64_t)m(-7, -3) == -1);
  assert((int64_t)m(0, 5) == 0);
  assert((int64_t)m(1 << 40, 3) == 1);            // (2^40) mod 3 = 1
  assert((int64_t)m(-4611686018427387904LL, 7) == -4);  // 2^62 mod 7 = 4
  assert((int64_t)m(4611686018427387904LL, -7) == 4);
  assert((int64_t)m(-9223372036854775807LL - 1, 2) == 0);  // INT64_MIN even
  // Soft-algo path: |INT64_MIN| stays INT64_MIN as unsigned (2^63), remainder
  // with b'=1 is 0, sign reapplied -> 0 (not the x86 idiv INT_MIN quirk).
  assert((int64_t)m(-9223372036854775807LL - 1, -1) == 0);

  // unsigned pair: values from exact arithmetic; also a==2^64-1 boundary.
  assert(guest::__udivdi3(1ULL << 40, 3) == ((1ULL << 40) / 3));
  assert(guest::__umoddi3(1ULL << 40, 3) == ((1ULL << 40) % 3));
  assert(guest::__udivdi3(18446744073709551615ULL, 7) == (18446744073709551615ULL / 7));
  assert(guest::__umoddi3(18446744073709551615ULL, 7) == (18446744073709551615ULL % 7));
  assert(guest::__udivdi3(1ULL << 63, 1ULL << 31) == (1ULL << 32));
  assert(guest::__umoddi3(4611686018427387904ULL, 7) == 4);
  return 0;
}
