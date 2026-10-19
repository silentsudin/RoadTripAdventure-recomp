// Pins W11-25 audit fixes:
// MatchSlotPairState (0x0023db80) — MIPS uses lb (signed BYTE) loads for all four
// DAT_0177fdd4..fdd7 slots; two were transliterated as uint32 reads (overlapping
// the next slot and comparing 4-byte values against char params). Truth table
// from the flattened bnel/beq chain at 0x23db8c: pair1 match -> 1, pair2 -> 2,
// neither -> 0.
#include "guest/guest_shim.h"

#include <cassert>
#include <cstring>

namespace guest {
uint32_t MatchSlotPairState(uint32_t = 0, uint32_t = 0, ...);
}

int main() {
  std::memset(rtguest::kRam, 0, RT_GUEST_RAM_SIZE);
  *(uint32_t*)rtguest::G(0x177fdd4) = 0x41 | ('B' << 8) | ('C' << 16) | ('D' << 24);
  assert(guest::MatchSlotPairState('A', 'B') == 1);  // first pair matches
  assert(guest::MatchSlotPairState('C', 'D') == 2);  // second pair matches
  assert(guest::MatchSlotPairState('X', 'Y') == 0);  // neither
}
