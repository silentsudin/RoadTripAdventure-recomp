// guest:0x0028df38 __udivdi3 — authored libgcc-equivalent body (see note)
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// pseudocode: ulong __udivdi3(long, long)
// Authored 2026-10-20 (cycle-3, moddi3_test pin): the transliterated long
// division loop mis-computes (LAB/goto defects survived the byte-table and
// (ulong) cast fixes). The externally visible semantics of this libgcc
// helper (fn/0028df38.c) is exact unsigned 64-bit division; a zero divisor
// break-traps (trap(7), fn/0028df38.c:42/75).
// shortcut: div0 return value is guest garbage (pseudocode continues after
// the trap); revisit only if a caller ever divides by zero.
uint64_t __udivdi3(uint64_t param_1, uint64_t param_2, ...) {
  rtguest::FrameGuard rt_fg(0);
  if (param_2 == 0) {
    trap(7);
    return 0xFFFFFFFFFFFFFFFFULL;
  }
  return param_1 / param_2;
}

RT_GUEST_FN(0x0028df38, guest::__udivdi3)

}  // namespace guest
