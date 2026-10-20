// guest:0x00241e90 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t townMapCoins(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(96);
  uint uVar1;
  int iVar2;
  long lVar3;
  int iVar4;
  int iVar5;
uint32_t auStack_60 = rtguest::allocFrame(16);
  
  iVar2 = 1;
  iVar4 = 0;
  do {
    iVar5 = iVar2;
    lVar3 = TestFlagFca8Clear(iVar5);
    if (lVar3 == 0) {
      uVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002a397c + iVar4 * 0x10)));
      lVar3 = tileSlotLoaded(param_2,uVar1);
      if ((lVar3 != 0) &&
         (lVar3 = tileSlotOrigin(auStack_60,0x2a3970 + iVar4 * 0x10,uVar1 & 3), lVar3 != 0
         )) {
        mapDot(param_1,auStack_60,param_2);
      }
    }
    iVar2 = iVar5 + 1;
    iVar4 = iVar5;
  } while (iVar5 < 100);
  return 0;
}


RT_GUEST_FN(0x00241e90, guest::townMapCoins)

}  // namespace guest
