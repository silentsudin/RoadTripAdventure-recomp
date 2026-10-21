// guest:0x002318b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t raceHudElementH(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(128);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x38, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0x70, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x48, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x58, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x68, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x78) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x7c) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(8, (uint32_t)param_2);  // walk: = param_2
  }
    uint32_t uStack_80 = rtguest::allocFrame(8);
    uint32_t uStack_78 = rtguest::allocFrame(8);
    uint32_t uStack_70 = rtguest::allocFrame(8);
    uint32_t uStack_68 = rtguest::allocFrame(8);
    uint32_t iStack_60 = rtguest::allocFrame(4);
  undefined4 uVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  
  (*(int32_t*)rtguest::G((uint32_t)iStack_60)) = (int)param_3 + -1;
  if (param_3 != 0) {
    do {
      if ((*(int32_t*)rtguest::G((uint32_t)param_2)) == 1) {
        (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x1800000020;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0xd800000000;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x2000000070;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
        RT_CALLRA(0x231958, guest::uiArenaSetTexture(param_1,0x29f5f8,1,2));
        RT_CALLRA(0x231964, guest::uiArenaDataTag(param_1,1));
        RT_CALLRA(0x231980, guest::uiArenaSprites(param_1,uStack_80,1,0,0,0));
        (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x4100000062;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0xc2000000a0;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1400000018;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
        RT_CALLRA(0x2319e0, guest::uiArenaSetTexture(param_1,0x29f5f8,1,1));
        RT_CALLRA(0x2319ec, guest::uiArenaDataTag(param_1,1));
        RT_CALLRA(0x231a08, guest::uiArenaSprites(param_1,uStack_80,1,0,0,0));
        (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x1800000232;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x68000000c8;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x3000000030;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
        RT_CALLRA(0x231a68, guest::uiArenaSetTexture(param_1,0x29f5f8,1,4));
        RT_CALLRA(0x231a74, guest::uiArenaDataTag(param_1,1));
        RT_CALLRA(0x231a90, guest::uiArenaSprites(param_1,uStack_80,1,0,0,0));
        uVar1 = (*(uint32_t*)rtguest::G(0x1780a28));
        uVar2 = RT_CALLRA(0x231aa4, guest::IntToPackedBCD((*(uint32_t*)rtguest::G(0x1780a28))));
        iVar3 = RT_CALLRA(0x231ab0, guest::CountDecimalDigits(uVar1));
        RT_CALLRA(0x231acc, guest::uiArenaSetTexture(param_1,0x29f5f8,1,1));
        RT_CALLRA(0x231ad8, guest::uiArenaDataTag(param_1,iVar3));
        if (iVar3 < 1) {
          iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(3) * sizeof(int32_t)));
        }
        else {
          iVar5 = 0x52;
          do {
            uVar4 = uVar2 & 0xf;
            uVar2 = (int)uVar2 >> 4;
            iVar3 = iVar3 + -1;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = CONCAT44(0x41,iVar5);
            (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = CONCAT44(0xc2,uVar4 << 4);
            (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1400000010;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
            RT_CALLRA(0x231b44, guest::uiArenaSprites(param_1,uStack_80,1,0,0,0));
            iVar5 = iVar5 + -0xf;
          } while (iVar3 != 0);
          iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(3) * sizeof(int32_t)));
        }
        if (iVar3 < 0) {
          iVar3 = 0;
        }
        uVar2 = RT_CALLRA(0x231b60, guest::IntToPackedBCD(iVar3));
        iVar3 = RT_CALLRA(0x231b6c, guest::CountDecimalDigits(iVar3));
        RT_CALLRA(0x231b88, guest::uiArenaSetTexture(param_1,0x29f5f8,1,4));
        RT_CALLRA(0x231b94, guest::uiArenaDataTag(param_1,iVar3));
        if (0 < iVar3) {
          iVar5 = 0x20a;
          do {
            uVar4 = uVar2 & 0xf;
            uVar2 = (int)uVar2 >> 4;
            iVar3 = iVar3 + -1;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = CONCAT44(0x18,iVar5);
            (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = CONCAT44((uVar4 / 6) * 0x30 + 0x38,(uVar4 % 6) * 0x28);
            (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x3000000028;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
            RT_CALLRA(0x231c30, guest::uiArenaSprites(param_1,uStack_80,1,0,0,0));
            iVar5 = iVar5 + -0x25;
          } while (iVar3 != 0);
        }
      }
      (*(int32_t*)rtguest::G((uint32_t)iStack_60)) = (*(int32_t*)rtguest::G((uint32_t)iStack_60)) + -1;
      param_2 = param_2 + 4;
    } while ((*(int32_t*)rtguest::G((uint32_t)iStack_60)) != -1);
  }
  return 0;
}


RT_GUEST_FN(0x002318b0, guest::raceHudElementH)

}  // namespace guest
