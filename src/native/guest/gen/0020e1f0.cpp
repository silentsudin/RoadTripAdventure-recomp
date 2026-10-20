// guest:0x0020e1f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t cdReadEntry(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  long lVar1;
  
  (*(uint32_t*)rtguest::G(0x3356c8)) = (uint32_t)param_2;
  (*(uint32_t*)rtguest::G(0x3356cc)) = param_1;
  lVar1 = sceCdRead((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 4))),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))),param_2,0x333380);
  (*(uint32_t*)rtguest::G(0x3356c4)) = (uint32_t)(lVar1 != 0);
  return (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x3356cc)) + 8))) << 0xb;
}


RT_GUEST_FN(0x0020e1f0, guest::cdReadEntry)

}  // namespace guest
