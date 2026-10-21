// guest:0x002119a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawMenuGridBorder(uint64_t param_1, uint32_t param_2, uint32_t param_3, uint64_t param_4, ...) {
  rtguest::FrameGuard rt_fg(128);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x38, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x48, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x58, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x68, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0x70, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x78) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x7c) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_3);  // walk: = param_3
    rtguest::setS32(1, (uint32_t)param_2);  // walk: = param_2
  }
    uint32_t uStack_80 = rtguest::allocFrame(8);
    uint32_t uStack_78 = rtguest::allocFrame(8);
    uint32_t uStack_70 = rtguest::allocFrame(8);
    uint32_t uStack_68 = rtguest::allocFrame(8);
    uint32_t iStack_60 = rtguest::allocFrame(4);
  uint uVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  (*(int32_t*)rtguest::G((uint32_t)iStack_60)) = (int)param_4 + -1;
  if (param_4 != 0) {
    do {
      if (((*(uint32_t*)rtguest::G((uint32_t)param_3)) & 1) != 0) {
        iVar4 = 0;
        RT_CALLRA(0x211a08, guest::uiArenaSetTexture(param_2,0x29c030,0,0));
        RT_CALLRA(0x211a18, guest::uiArenaDataTag(param_2,0x7b));
        (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0xb8;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x4900000018;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
        RT_CALLRA(0x211a80, guest::uiArenaSprites(param_2,uStack_80,1,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(3) * sizeof(uint32_t)))));
        (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x108;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0xd8;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x4900000020;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
        RT_CALLRA(0x211ae8, guest::uiArenaSprites(param_2,uStack_80,1,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(3) * sizeof(uint32_t)))));
        (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x128;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x49000000b8;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x2100000018;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
        iVar5 = 0x1d;
        RT_CALLRA(0x211b50, guest::uiArenaSprites(param_2,uStack_80,1,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(3) * sizeof(uint32_t)))));
        uVar2 = (*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t)));
        do {
          uVar1 = iVar4 + 0x18;
          iVar5 = iVar5 + -1;
          iVar4 = iVar4 + 8;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = (uint32_t)uVar1;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0xd0;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x4900000008;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
          RT_CALLRA(0x211b9c, guest::uiArenaSprites(param_2,uStack_80,1,uVar2,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(3) * sizeof(uint32_t)))));
          uVar2 = (*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t)));
        } while (-1 < iVar5);
        iVar4 = 0;
        iVar5 = 4;
        do {
          (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = (uint32_t)(iVar4 + 0x140);
          iVar5 = iVar5 + -1;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x49000000d0;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x2100000008;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
          RT_CALLRA(0x211c00, guest::uiArenaSprites(param_2,uStack_80,1,uVar2,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(3) * sizeof(uint32_t)))));
          (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = (uint32_t)(iVar4 + 0x148);
          (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x49000000d0;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x2100000008;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
          RT_CALLRA(0x211c40, guest::uiArenaSprites(param_2,uStack_80,1,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(3) * sizeof(uint32_t)))));
          (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = (uint32_t)(iVar4 + 0x150);
          (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x49000000d8;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x210000000e;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
          RT_CALLRA(0x211c88, guest::uiArenaSprites(param_2,uStack_80,1,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(3) * sizeof(uint32_t)))));
          (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = (uint32_t)(iVar4 + 0x15e);
          (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x49000000d0;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x2100000008;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
          RT_CALLRA(0x211cc8, guest::uiArenaSprites(param_2,uStack_80,1,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(3) * sizeof(uint32_t)))));
          (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = (uint32_t)(iVar4 + 0x166);
          (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x49000000d0;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x2100000008;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
          RT_CALLRA(0x211d08, guest::uiArenaSprites(param_2,uStack_80,1,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(3) * sizeof(uint32_t)))));
          uVar2 = iVar4 + 0x16e;
          iVar4 = iVar4 + 0x40;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = (uint32_t)uVar2;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x49000000e6;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x2100000012;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
          RT_CALLRA(0x211d54, guest::uiArenaSprites(param_2,uStack_80,1,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(3) * sizeof(uint32_t)))));
          uVar2 = (*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t)));
        } while (-1 < iVar5);
        iVar4 = 0;
        iVar5 = 9;
        while( true ) {
          iVar5 = iVar5 + -1;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = CONCAT44(0x1a0,iVar4);
          (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x49000000d0;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x2100000008;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
          RT_CALLRA(0x211db4, guest::uiArenaSprites(param_2,uStack_80,1,uVar2,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(3) * sizeof(uint32_t)))));
          (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = CONCAT44(0x1a0,iVar4 + 8);
          (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x49000000d0;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x2100000008;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
          RT_CALLRA(0x211df4, guest::uiArenaSprites(param_2,uStack_80,1,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(3) * sizeof(uint32_t)))));
          (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = CONCAT44(0x1a0,iVar4 + 0x10);
          (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x49000000d8;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x210000000e;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
          RT_CALLRA(0x211e3c, guest::uiArenaSprites(param_2,uStack_80,1,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(3) * sizeof(uint32_t)))));
          (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = CONCAT44(0x1a0,iVar4 + 0x1e);
          (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x49000000d0;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x2100000008;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
          RT_CALLRA(0x211e7c, guest::uiArenaSprites(param_2,uStack_80,1,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(3) * sizeof(uint32_t)))));
          (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = CONCAT44(0x1a0,iVar4 + 0x26);
          (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x49000000d0;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x2100000008;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
          RT_CALLRA(0x211ebc, guest::uiArenaSprites(param_2,uStack_80,1,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(3) * sizeof(uint32_t)))));
          iVar3 = iVar4 + 0x2e;
          iVar4 = iVar4 + 0x40;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = CONCAT44(0x1a0,iVar3);
          (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x49000000e6;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x2100000012;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
          RT_CALLRA(0x211f08, guest::uiArenaSprites(param_2,uStack_80,1,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(3) * sizeof(uint32_t)))));
          if (iVar5 < 0) break;
          uVar2 = (*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t)));
        }
      }
      (*(int32_t*)rtguest::G((uint32_t)iStack_60)) = (*(int32_t*)rtguest::G((uint32_t)iStack_60)) + -1;
      param_3 = param_3 + 4;
    } while ((*(int32_t*)rtguest::G((uint32_t)iStack_60)) != -1);
  }
  return 0;
}


RT_GUEST_FN(0x002119a8, guest::DrawMenuGridBorder)

}  // namespace guest
