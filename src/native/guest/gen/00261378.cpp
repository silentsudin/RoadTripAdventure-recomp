// guest:0x00261378 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t navigateRouletteTargetGrid(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  ushort uVar1;
  ushort uVar2;
  int iVar3;
  
  uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x30)));
  if (((uVar1 & 0x1000) == 0) &&
     ((uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x34))), (uVar2 & 0x1000) == 0 ||
      (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002d9994 + param_2 * 0x30))) & 0x1000) != 0)))) {
    if (((uVar1 & 0x4000) == 0) &&
       (((uVar2 & 0x4000) == 0 || (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002d9994 + param_2 * 0x30))) & 0x4000) != 0)))) {
      if (((uVar1 & 0x8000) == 0) &&
         (((uVar2 & 0x8000) == 0 || (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002d9994 + param_2 * 0x30))) & 0x8000) != 0)))) {
        if ((uVar1 & 0x2000) == 0) {
          if ((uVar2 & 0x2000) == 0) {
            return param_2;
          }
          if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002d9994 + param_2 * 0x30))) & 0x2000) != 0) {
            return param_2;
          }
        }
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002d9990 + param_2 * 0x30)));
      }
      else {
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002d998c + param_2 * 0x30)));
      }
    }
    else {
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002d9988 + param_2 * 0x30)));
    }
  }
  else {
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002d9984 + param_2 * 0x30)));
  }
  if (iVar3 != param_2) {
    PlayUiSound(0x1d);
  }
  return iVar3;
}


RT_GUEST_FN(0x00261378, guest::navigateRouletteTargetGrid)

}  // namespace guest
