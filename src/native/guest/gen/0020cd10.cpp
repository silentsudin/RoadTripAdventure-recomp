// guest:0x0020cd10 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateInputMapping(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  long lVar1;
  int iVar2;
  
  if ((*(uint32_t*)rtguest::G(0x3356c0)) == 0) {
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) = (char)((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) >> 8);
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
  }
  else if ((*(uint32_t*)rtguest::G(0x3356c0)) == (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) << 1) {
    (*(uint32_t*)rtguest::G(0x3356c0)) = 0xffffffff;
    return false;
  }
  iVar2 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) + (*(uint32_t*)rtguest::G(0x3356c0)) / 2;
  lVar1 = loadCarModel(0x3fbb80 + iVar2 * 0x34000,iVar2,
                       (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G((uint32_t)0x177ea50 + (uint32_t)(iVar2) * 4)) + 4))));
  return lVar1 == 0;
}


RT_GUEST_FN(0x0020cd10, guest::UpdateInputMapping)

}  // namespace guest
