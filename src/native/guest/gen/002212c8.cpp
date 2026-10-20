// guest:0x002212c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetVifFillDirect(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  (*(uint64_t*)rtguest::G(0x0029d130)) = (uint64_t)((int)param_1 << 0x10 | (int)param_1 << 8) | param_1;
  vifDirect(0x29d0f0,6);
  return 0;
}


RT_GUEST_FN(0x002212c8, guest::SetVifFillDirect)

}  // namespace guest
