// guest:0x0027afd8 __umoddi3 — authored libgcc-equivalent body (see note)
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// pseudocode: ulong __umoddi3(long, long)
// Authored 2026-10-20 (cycle-3, moddi3_test pin): same transliterated
// long-division defect family as __udivdi3 (0x0028df38). Externally visible
// semantics (fn/0027afd8.c): exact unsigned 64-bit remainder; zero divisor
// break-traps (trap(7)); callers include _printf/_vfprintf_r.
// shortcut: div0 return value is guest garbage; revisit only on a div0 call.
uint64_t __umoddi3(uint64_t param_1, uint64_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  if (param_2 == 0) {
    trap(7);
    return 0xFFFFFFFFFFFFFFFFULL;
  }
  return param_1 % param_2;
}

RT_GUEST_FN(0x0027afd8, guest::__umoddi3)

}  // namespace guest
