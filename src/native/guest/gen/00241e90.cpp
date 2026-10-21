// guest:0x00241e90 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t townMapCoins(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x28, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x48) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4c) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x38, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x40, 8);
    rtguest::rtSpillF(rt_fg, 4, rt_sv, 0x50);
    rtguest::gS[1] = 0x0000000000000000ull;  // walk const
    rtguest::gS[3] = 0x0000000000000001ull;  // walk const
    rtguest::setS32(4, (uint32_t)param_1);  // walk: = param_1
    rtguest::gS[5] = 0x00000000002a0000ull;  // walk const
    rtguest::gS[6] = 0x00000000002a397cull;  // walk const
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
