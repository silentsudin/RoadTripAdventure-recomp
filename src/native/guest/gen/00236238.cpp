// guest:0x00236238 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SumPlayerPoints(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  
  iVar1 = 0;
  if ((*(uint8_t*)rtguest::G(0x178700a)) < 6) {
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0029fcb8 + ((uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 3))) * 6 + (uint32_t)(*(uint8_t*)rtguest::G(0x178700a))) * 4)));
  }
  if (((*(int8_t*)rtguest::G(0x177fdd4)) != '\0') && ((*(uint8_t*)rtguest::G(0x178700e)) < 6)) {
    iVar1 = iVar1 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0029fcb8 +
                            ((uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 3))) * 6 + (uint32_t)(*(uint8_t*)rtguest::G(0x178700e))) * 4)));
  }
  if (((*(int8_t*)rtguest::G(0x177fdd6)) != '\0') && ((*(uint8_t*)rtguest::G(0x1787012)) < 6)) {
    iVar1 = iVar1 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0029fcb8 +
                            ((uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 3))) * 6 + (uint32_t)(*(uint8_t*)rtguest::G(0x1787012))) * 4)));
  }
  return iVar1;
}


RT_GUEST_FN(0x00236238, guest::SumPlayerPoints)

}  // namespace guest
