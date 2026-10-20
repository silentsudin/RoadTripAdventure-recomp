// guest:0x00270d50 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t textGridCreate(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(96);
  long lVar1;
  uint uVar2;
uint32_t piVar3;
  
  if ((param_3 < 0x51) && (param_4 < 0x41)) {
    piVar3 = 0x01773040;
    if ((*(uint32_t*)rtguest::G(0x1773048)) != 0) {
      for (uVar2 = 1; piVar3 = (uint32_t)0x0, uVar2 < 4; uVar2 = uVar2 + 1) {
        if ((*(uint32_t*)rtguest::G((uint32_t)0x1773048 + (uint32_t)(uVar2 * 0x16) * 4)) == 0) {
          piVar3 = 0x01773040 + uVar2 * 0x16;
          break;
        }
      }
    }
    if (piVar3 == (uint32_t)0x0) {
      return (uint32_t)0x0;
    }
    lVar1 = heapAllocBlock((int)param_3 * (int)param_4 * 2);
    (*(int32_t*)rtguest::G((uint32_t)piVar3 + (uint32_t)(2) * sizeof(int32_t))) = (int)lVar1;
    if (lVar1 != 0) {
      (*(int32_t*)rtguest::G((uint32_t)piVar3)) = (int)param_3;
      (*(int32_t*)rtguest::G((uint32_t)piVar3 + (uint32_t)(1) * sizeof(int32_t))) = (int)param_4;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(piVar3 + 3))) = 7;
      textGridInitAttrs(piVar3 + 6,param_1,param_2);
      textGridClear(piVar3);
      return piVar3;
    }
  }
  return (uint32_t)0x0;
}


RT_GUEST_FN(0x00270d50, guest::textGridCreate)

}  // namespace guest
