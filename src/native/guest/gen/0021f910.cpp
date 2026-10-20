// guest:0x0021f910 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetViewDistanceGlobals(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(144);
  (*(uint32_t*)rtguest::G(0x3356e0)) = param_1;
  (*(uint32_t*)rtguest::G(0x3356e4)) = param_2;
  return 0;
}


RT_GUEST_FN(0x0021f910, guest::SetViewDistanceGlobals)

}  // namespace guest
