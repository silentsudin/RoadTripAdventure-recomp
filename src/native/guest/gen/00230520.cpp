// guest:0x00230520 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t raceHudElementE(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
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
        uiArenaSetTexture(param_1, 0x29f5f8, 0, 3);
        iVar6 = 0;
        uiArenaDataTag(param_1, 1);
        uiArenaSprites(param_1, uStack_80, 1, iVar5, iVar4, 0);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x500000028;
        iVar3 = 0x34;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) =
             ((uint64_t)(uint32_t)0x91 << 32) |
             (uint32_t)((*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(3) * 4)) + 1) * 0x14;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1e00000014;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
        uiArenaSetTexture(param_1, 0x29f5f8, 1, 1);
        uiArenaDataTag(param_1, 1);
        uiArenaSprites(param_1, uStack_80, 1, iVar5, iVar4, 0);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x2e0000004e;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x91000000c8;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1e00000014;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
        uiArenaSetTexture(param_1, 0x29f5f8, 1, 1);
        uiArenaDataTag(param_1, 1);
        uiArenaSprites(param_1, uStack_80, 1, iVar5, iVar4, 0);
        uVar1 = IntToPackedBCD((*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(4) * 4)));
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
          uiArenaSetTexture(param_1, 0x29f5f8, 1, 1);
          uiArenaDataTag(param_1, 1);
          uiArenaSprites(param_1, uStack_80, 1, iVar5, iVar4, 0);
          if (uVar1 == 0) break;
          iVar6 = iVar6 + 1;
        } while (iVar6 < 4);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x5000000004;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x59000000b9;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1e00000046;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
        uiArenaSetTexture(param_1, 0x29f5f8, 0, 4);
        uiArenaDataTag(param_1, 1);
        iVar6 = 0;
        uiArenaSprites(param_1, uStack_80, 1, iVar5, iVar4, 0);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x500000004e;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) =
             ((uint64_t)(uint32_t)0x91 << 32) |
             (uint32_t)((*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(6) * 4)) * 0x14);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1e00000014;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
        uiArenaSetTexture(param_1, 0x29f5f8, 1, 1);
        uiArenaDataTag(param_1, 1);
        uiArenaSprites(param_1, uStack_80, 1, iVar5, iVar4, 0);
        iVar5 = ((*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * 4)) + 0x20a);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0xa00000016;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x31000000c9;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1e00000036;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
        iVar4 = ((*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * 4)) + 0x14);
        uiArenaSetTexture(param_1, 0x29f5f8, 0, 4);
        uiArenaDataTag(param_1, 1);
        uiArenaSprites(param_1, uStack_80, 1, iVar5, iVar4, 0);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x2e0000004e;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x91000000c8;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1e00000014;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
        uiArenaSetTexture(param_1, 0x29f5f8, 1, 1);
        uiArenaDataTag(param_1, 1);
        uiArenaSprites(param_1, uStack_80, 1, iVar5, iVar4, 0);
        uVar1 = IntToPackedBCD((*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(5) * 4)));
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
          uiArenaSetTexture(param_1, 0x29f5f8, 1, 1);
          uiArenaDataTag(param_1, 1);
          uiArenaSprites(param_1, uStack_80, 1, iVar5, iVar4, 0);
          if (uVar1 == 0) break;
          iVar6 = iVar6 + 1;
          iVar3 = iVar3 + -0x18;
        } while (iVar6 < 4);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x5000000000;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x79000000b0;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x200000003c;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
        uiArenaSetTexture(param_1, 0x29f5f8, 0, 4);
        iVar6 = 0;
        uiArenaDataTag(param_1, 1);
        uiArenaSprites(param_1, uStack_80, 1, iVar5, iVar4, 0);
        iVar3 = 0x46;
        uVar1 = IntToPackedBCD((*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(7) * 4)) + 1);
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
          uiArenaSetTexture(param_1, 0x29f5f8, 1, 1);
          uiArenaDataTag(param_1, 1);
          uiArenaSprites(param_1, uStack_80, 1, iVar5, iVar4, 0);
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
