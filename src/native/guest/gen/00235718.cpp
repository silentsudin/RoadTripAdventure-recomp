// guest:0x00235718 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetObjectFields848C8480(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x848c))) = param_2;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8480))) = param_3;
  return 0;
}


RT_GUEST_FN(0x00235718, guest::SetObjectFields848C8480)

}  // namespace guest
