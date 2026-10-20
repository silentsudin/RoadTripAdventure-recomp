// guest:0x0028d8a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
// __moddi3 (libgcc signed 64-bit remainder): the pseudocode is the classic
// shift/subtract long-division loop with sign flags; only externally visible
// semantics: signed remainder, break/trap(7) on zero divisor (the pseudocode
// guards every 32-bit divide with if(x==0) trap(7)).  The transliterator's
// 32-bit-ized + return-less rendition is semantically wrong, so this TU is
// authored to those semantics (same treatment as isinf/isnan class).
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint64_t __moddi3(uint64_t param_1, uint64_t param_2) {
  rtguest::FrameGuard rt_fg(32);
  if (param_2 == 0) {
    trap(7);
    return 0;
  }
  int64_t q = (int64_t)param_1 % (int64_t)param_2;
  return (uint64_t)q;
}


RT_GUEST_FN(0x0028d8a8, guest::__moddi3)

}  // namespace guest
