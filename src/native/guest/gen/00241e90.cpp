// guest:0x00241e90 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t townMapCoins(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x48) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4c) = 0x00000000u;
  }
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
    lVar3 = RT_CALLRA(0x241ed8, guest::TestFlagFca8Clear(iVar5));
    if (lVar3 == 0) {
      uVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002a397c + iVar4 * 0x10)));
      lVar3 = RT_CALLRA(0x241ef8, guest::tileSlotLoaded(param_2,uVar1));
      if ((lVar3 != 0) &&
         (lVar3 = RT_CALLRA(0x241f10, guest::tileSlotOrigin(auStack_60,0x2a3970 + iVar4 * 0x10,uVar1 & 3)), lVar3 != 0
         )) {
        RT_CALLRA(0x241f24, guest::mapDot(param_1,auStack_60,param_2));
      }
    }
    iVar2 = iVar5 + 1;
    iVar4 = iVar5;
  } while (iVar5 < 100);
  return 0;
}


RT_GUEST_FN(0x00241e90, guest::townMapCoins)

}  // namespace guest
