// guest:0x00230520 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t raceHudElementE(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(128);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x38, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x48, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x58, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x68, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0x70, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x78) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x7c) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(6, (uint32_t)param_2);  // walk: = param_2
  }
  uint uVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  uint32_t uStack_80 = rtguest::allocFrame(8);
  uint32_t uStack_78 = rtguest::allocFrame(8);
  uint32_t uStack_70 = rtguest::allocFrame(8);
  uint32_t uStack_68 = rtguest::allocFrame(8);
  uint32_t iStack_60 = rtguest::allocFrame(4);

  (*(int32_t*)rtguest::G((uint32_t)iStack_60)) = ((int)param_3 + -1);
  if (param_3 != 0) {
    do {
      if ((*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(0) * 4)) != 0) {
        (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x1e;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x1000000ad;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x2800000028;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
        iVar4 = ((*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * 4)) + 0x14);
        iVar5 = ((*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * 4)) + 0x28);
        RT_CALLRA(0x2305d0, guest::uiArenaSetTexture(param_1, 0x29f5f8, 0, 3));
        iVar6 = 0;
        RT_CALLRA(0x2305e0, guest::uiArenaDataTag(param_1, 1));
        RT_CALLRA(0x230600, guest::uiArenaSprites(param_1, uStack_80, 1, iVar5, iVar4, 0));
        (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x500000028;
        iVar3 = 0x34;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) =
             ((uint64_t)(uint32_t)0x91 << 32) |
             (uint32_t)((*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(3) * 4)) + 1) * 0x14;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1e00000014;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
        RT_CALLRA(0x230668, guest::uiArenaSetTexture(param_1, 0x29f5f8, 1, 1));
        RT_CALLRA(0x230674, guest::uiArenaDataTag(param_1, 1));
        RT_CALLRA(0x230690, guest::uiArenaSprites(param_1, uStack_80, 1, iVar5, iVar4, 0));
        (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x2e0000004e;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x91000000c8;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1e00000014;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
        RT_CALLRA(0x2306ec, guest::uiArenaSetTexture(param_1, 0x29f5f8, 1, 1));
        RT_CALLRA(0x2306f8, guest::uiArenaDataTag(param_1, 1));
        RT_CALLRA(0x230714, guest::uiArenaSprites(param_1, uStack_80, 1, iVar5, iVar4, 0));
        uVar1 = RT_CALLRA(0x23071c, guest::IntToPackedBCD((*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(4) * 4))));
        do {
          uVar2 = uVar1 & 0xf;
          uVar1 = (int)uVar1 >> 4;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) =
               ((uint64_t)(uint32_t)0x2e << 32) | (uint32_t)iVar3;
          iVar3 = iVar3 + -0x18;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) =
               ((uint64_t)(uint32_t)0x91 << 32) | (uint32_t)(uVar2 * 0x14);
          (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1e00000014;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
          RT_CALLRA(0x23077c, guest::uiArenaSetTexture(param_1, 0x29f5f8, 1, 1));
          RT_CALLRA(0x230788, guest::uiArenaDataTag(param_1, 1));
          RT_CALLRA(0x2307a4, guest::uiArenaSprites(param_1, uStack_80, 1, iVar5, iVar4, 0));
          if (uVar1 == 0) break;
          iVar6 = iVar6 + 1;
        } while (iVar6 < 4);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x5000000004;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x59000000b9;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1e00000046;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
        RT_CALLRA(0x230818, guest::uiArenaSetTexture(param_1, 0x29f5f8, 0, 4));
        RT_CALLRA(0x230828, guest::uiArenaDataTag(param_1, 1));
        iVar6 = 0;
        RT_CALLRA(0x230848, guest::uiArenaSprites(param_1, uStack_80, 1, iVar5, iVar4, 0));
        (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x500000004e;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) =
             ((uint64_t)(uint32_t)0x91 << 32) |
             (uint32_t)((*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(6) * 4)) * 0x14);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1e00000014;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
        RT_CALLRA(0x2308a4, guest::uiArenaSetTexture(param_1, 0x29f5f8, 1, 1));
        RT_CALLRA(0x2308b0, guest::uiArenaDataTag(param_1, 1));
        RT_CALLRA(0x2308cc, guest::uiArenaSprites(param_1, uStack_80, 1, iVar5, iVar4, 0));
        iVar5 = ((*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * 4)) + 0x20a);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0xa00000016;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x31000000c9;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1e00000036;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
        iVar4 = ((*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * 4)) + 0x14);
        RT_CALLRA(0x230938, guest::uiArenaSetTexture(param_1, 0x29f5f8, 0, 4));
        RT_CALLRA(0x230944, guest::uiArenaDataTag(param_1, 1));
        RT_CALLRA(0x230960, guest::uiArenaSprites(param_1, uStack_80, 1, iVar5, iVar4, 0));
        (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x2e0000004e;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x91000000c8;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1e00000014;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
        RT_CALLRA(0x2309bc, guest::uiArenaSetTexture(param_1, 0x29f5f8, 1, 1));
        RT_CALLRA(0x2309c8, guest::uiArenaDataTag(param_1, 1));
        RT_CALLRA(0x2309e4, guest::uiArenaSprites(param_1, uStack_80, 1, iVar5, iVar4, 0));
        uVar1 = RT_CALLRA(0x2309ec, guest::IntToPackedBCD((*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(5) * 4))));
        iVar3 = 0x34;
        do {
          uVar2 = uVar1 & 0xf;
          uVar1 = (int)uVar1 >> 4;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) =
               ((uint64_t)(uint32_t)0x2e << 32) | (uint32_t)iVar3;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) =
               ((uint64_t)(uint32_t)0x91 << 32) | (uint32_t)(uVar2 * 0x14);
          (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1e00000014;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
          RT_CALLRA(0x230a50, guest::uiArenaSetTexture(param_1, 0x29f5f8, 1, 1));
          RT_CALLRA(0x230a5c, guest::uiArenaDataTag(param_1, 1));
          RT_CALLRA(0x230a78, guest::uiArenaSprites(param_1, uStack_80, 1, iVar5, iVar4, 0));
          if (uVar1 == 0) break;
          iVar6 = iVar6 + 1;
          iVar3 = iVar3 + -0x18;
        } while (iVar6 < 4);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x5000000000;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x79000000b0;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x200000003c;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
        RT_CALLRA(0x230aec, guest::uiArenaSetTexture(param_1, 0x29f5f8, 0, 4));
        iVar6 = 0;
        RT_CALLRA(0x230afc, guest::uiArenaDataTag(param_1, 1));
        RT_CALLRA(0x230b1c, guest::uiArenaSprites(param_1, uStack_80, 1, iVar5, iVar4, 0));
        iVar3 = 0x46;
        uVar1 = RT_CALLRA(0x230b30, guest::IntToPackedBCD((*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(7) * 4)) + 1));
        do {
          uVar2 = uVar1 & 0xf;
          uVar1 = (int)uVar1 >> 4;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) =
               ((uint64_t)(uint32_t)0x50 << 32) | (uint32_t)iVar3;
          iVar3 = iVar3 + -0x14;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) =
               ((uint64_t)(uint32_t)0x91 << 32) | (uint32_t)(uVar2 * 0x14);
          (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1e00000014;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
          RT_CALLRA(0x230b8c, guest::uiArenaSetTexture(param_1, 0x29f5f8, 1, 1));
          RT_CALLRA(0x230b98, guest::uiArenaDataTag(param_1, 1));
          RT_CALLRA(0x230bb4, guest::uiArenaSprites(param_1, uStack_80, 1, iVar5, iVar4, 0));
          if (uVar1 == 0) break;
          iVar6 = iVar6 + 1;
        } while (iVar6 < 4);
      }
      param_2 = param_2 + 0x24;  // int* element step 9
      (*(int32_t*)rtguest::G((uint32_t)iStack_60)) = ((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + -1);
    } while ((*(int32_t*)rtguest::G((uint32_t)iStack_60)) != -1);
  }
  return 0;
}


RT_GUEST_FN(0x00230520, guest::raceHudElementE)

}  // namespace guest
