// Pins W11-31: ScaleShortByMatrix (0x24fc98) — first TU to exercise the fixed
// vu0ScaleVector float-value convention (2026-10-19 defect class: raw-u32
// reads passed to the helper's float scalar param value-converted the BIT
// PATTERN instead of the float; guest mov/lwc1 keeps the value), and
// AppendQueuedByte (0x23df38) — 128-byte ring at DAT_0177fd08 with the
// full-queue left-shift path.
#include "guest/guest_shim.h"

#include <cassert>
#include <cstring>

namespace guest {
uint32_t ScaleShortByMatrix(uint32_t q0 = 0, ...);
uint32_t AppendQueuedByte(uint32_t q0 = 0, ...);
}  // namespace guest

int main() {
  std::memset(rtguest::kRam, 0, sizeof(rtguest::kRam));

  // ScaleShortByMatrix: (int)2 -> itof4 (fn/00272a28.c _vitof4 = /16) ->
  // (0.125,0,0,0) x DAT_00332f98=3.0f -> 0.375f
  *(float*)rtguest::G(0x00332f98) = 3.0f;
  assert(guest::ScaleShortByMatrix(2) == 0x3EC00000u);
  // Negative short input: a0 holds the sign-extended word 0xFFFFFFFE for -2;
  // ScaleShortByMatrix takes a register value, not a truncated half.
  assert(guest::ScaleShortByMatrix(0xFFFFFFFEu) == 0xBEC00000u);

  // AppendQueuedByte: append path
  *(uint8_t*)rtguest::G(0x0177fd88) = 0;
  guest::AppendQueuedByte(0x41);
  guest::AppendQueuedByte(0x42);
  assert(*(uint8_t*)rtguest::G(0x0177fd08) == 0x41);
  assert(*(uint8_t*)rtguest::G(0x0177fd09) == 0x42);
  assert(*(uint8_t*)rtguest::G(0x0177fd88) == 2);

  // Full-queue shift: count 0x80 -> bytes[1..0x7e] move down, byte lands at +0x7f
  *(uint8_t*)rtguest::G(0x0177fd88) = 0x80;
  for (int i = 0; i < 0x80; ++i) *(uint8_t*)rtguest::G(0x0177fd08 + i) = (uint8_t)i;
  guest::AppendQueuedByte(0x55);
  assert(*(uint8_t*)rtguest::G(0x0177fd08) == 1);
  assert(*(uint8_t*)rtguest::G(0x0177fd08 + 0x7e) == 0x7f);
  assert(*(uint8_t*)rtguest::G(0x0177fd87) == 0x55);
  assert(*(uint8_t*)rtguest::G(0x0177fd88) == 0x80);  // count untouched in shift path

  return 0;
}
