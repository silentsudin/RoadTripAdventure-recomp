// guest:0x00262358 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t setRouletteReelResult(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  
  iVar3 = (int)param_2;
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002d9db8 + iVar3 * 4)));
  if (iVar1 == 0) {
    iVar2 = -1;
    uVar4 = 0;
  }
  else {
    iVar2 = iVar3 + 1;
    if (param_2 < 10) {
      iVar2 = iVar3;
    }
    iVar2 = iVar2 % 2;
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8008)));
    uVar4 = 0;
    if (iVar3 < 0x12) {
      if (iVar3 == iVar1 + -1) {
        uVar4 = 1;
      }
    }
    else if (iVar3 < 0x15) {
      if (iVar3 == (iVar1 + -1) / 6 + 0x12) {
        uVar4 = 1;
      }
    }
    else {
      uVar4 = (uint32_t)(iVar3 == iVar2 + 0x15);
    }
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8028))) = uVar4;
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8020))) = iVar1;
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8024))) = iVar2;
  return 0;
}


RT_GUEST_FN(0x00262358, guest::setRouletteReelResult)

}  // namespace guest
