// guest:0x00231c90 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t raceHudElementI(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(128);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x38, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x58, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x48, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x68, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0x70, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x78) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x7c) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(5, (uint32_t)param_2);  // walk: = param_2
  }
    uint32_t uStack_80 = rtguest::allocFrame(8);
    uint32_t uStack_78 = rtguest::allocFrame(8);
    uint32_t uStack_70 = rtguest::allocFrame(8);
    uint32_t uStack_68 = rtguest::allocFrame(8);
    uint32_t iStack_60 = rtguest::allocFrame(4);
  uint uVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  
  (*(int32_t*)rtguest::G((uint32_t)iStack_60)) = (int)param_3 + -1;
  if (param_3 != 0) {
    do {
      if ((*(int32_t*)rtguest::G((uint32_t)param_2)) == 1) {
        (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x28000001c5;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x68000000a0;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x2600000038;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
        RT_CALLRA(0x231d38, guest::uiArenaSetTexture(param_1,0x29f5f8,1,9));
        RT_CALLRA(0x231d44, guest::uiArenaDataTag(param_1,1));
        RT_CALLRA(0x231d60, guest::uiArenaSprites(param_1,uStack_80,1,0,0,0));
        (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x3000000200;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x68000000d8;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1400000016;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
        RT_CALLRA(0x231dc0, guest::uiArenaSetTexture(param_1,0x29f5f8,1,9));
        RT_CALLRA(0x231dcc, guest::uiArenaDataTag(param_1,1));
        RT_CALLRA(0x231de8, guest::uiArenaSprites(param_1,uStack_80,1,0,0,0));
        iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(3) * sizeof(int32_t)));
        if (iVar2 < 0) {
          iVar2 = 0;
        }
        uVar1 = RT_CALLRA(0x231dfc, guest::IntToPackedBCD(iVar2));
        iVar2 = RT_CALLRA(0x231e08, guest::CountDecimalDigits(iVar2));
        RT_CALLRA(0x231e24, guest::uiArenaSetTexture(param_1,0x29f5f8,1,4));
        RT_CALLRA(0x231e30, guest::uiArenaDataTag(param_1,iVar2));
        if (0 < iVar2) {
          iVar4 = 0x240;
          do {
            uVar3 = uVar1 & 0xf;
            uVar1 = (int)uVar1 >> 4;
            iVar2 = iVar2 + -1;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = CONCAT44(0x22,iVar4);
            (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = CONCAT44((uVar3 / 6) * 0x30 + 0x38,(uVar3 % 6) * 0x28);
            (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x3000000028;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
            RT_CALLRA(0x231ec8, guest::uiArenaSprites(param_1,uStack_80,1,0,0,0));
            iVar4 = iVar4 + -0x25;
          } while (iVar2 != 0);
        }
      }
      (*(int32_t*)rtguest::G((uint32_t)iStack_60)) = (*(int32_t*)rtguest::G((uint32_t)iStack_60)) + -1;
      param_2 = param_2 + 4;
    } while ((*(int32_t*)rtguest::G((uint32_t)iStack_60)) != -1);
  }
  return 0;
}


RT_GUEST_FN(0x00231c90, guest::raceHudElementI)

}  // namespace guest
