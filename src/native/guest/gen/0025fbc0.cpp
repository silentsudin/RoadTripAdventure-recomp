// guest:0x0025fbc0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t carAiPathLookup(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
uint32_t piVar1;
  
  piVar1 = (uint32_t)((*(uint32_t*)rtguest::G((uint32_t)(0x2d98a8u + (uint32_t)(param_1) * 4))) + param_2 * 8);
  if (((*(uint32_t*)rtguest::G((uint32_t)(0x2d98a8u + (uint32_t)(param_1) * 4))) == (uint32_t)0x0) || ((*(int32_t*)rtguest::G((uint32_t)piVar1)) == 0)) {
    piVar1 = (uint32_t)0x0;
  }
  return piVar1;
}


RT_GUEST_FN(0x0025fbc0, guest::carAiPathLookup)

}  // namespace guest
