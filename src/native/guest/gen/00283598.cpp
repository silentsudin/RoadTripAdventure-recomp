// guest:0x00283598 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _mbtowc_r(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(16);
  uint uVar1;
uint32_t puVar2;
uint32_t auStack_10 = rtguest::allocFrame(16);
  
  uVar1 = 0;
  puVar2 = auStack_10;
  if (param_2 != (uint32_t)0x0) {
    puVar2 = param_2;
  }
  if ((param_3 != 0) && (uVar1 = 0xffffffff, param_4 != 0)) {
    (*(uint32_t*)rtguest::G((uint32_t)puVar2)) = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)param_3));
    uVar1 = (uint32_t)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)param_3)) != 0);
  }
  return uVar1;
}


RT_GUEST_FN(0x00283598, guest::_mbtowc_r)

}  // namespace guest
