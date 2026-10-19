// guest:0x00231f28 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t raceHudElementJ(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
    uint32_t uStack_100 = rtguest::allocFrame(8);
    uint32_t uStack_f8 = rtguest::allocFrame(8);
    uint32_t uStack_f0 = rtguest::allocFrame(8);
    uint32_t uStack_e8 = rtguest::allocFrame(8);
    uint32_t uStack_e0 = rtguest::allocFrame(4);
    uint32_t uStack_dc = rtguest::allocFrame(4);
    uint32_t iStack_d8 = rtguest::allocFrame(4);
    uint32_t iStack_d4 = rtguest::allocFrame(4);
    uint32_t uStack_d0 = rtguest::allocFrame(4);
    uint32_t uStack_cc = rtguest::allocFrame(4);
    uint32_t uStack_c8 = rtguest::allocFrame(4);
    uint32_t uStack_c4 = rtguest::allocFrame(4);
uint32_t auStack_c0 = rtguest::allocFrame(12);
    uint32_t uStack_b4 = rtguest::allocFrame(4);
    uint32_t uStack_b0 = rtguest::allocFrame(4);
    uint32_t uStack_ac = rtguest::allocFrame(4);
    uint32_t uStack_a8 = rtguest::allocFrame(4);
    uint32_t uStack_a4 = rtguest::allocFrame(4);
    uint32_t uStack_a0 = rtguest::allocFrame(4);
    uint32_t uStack_9c = rtguest::allocFrame(4);
    uint32_t uStack_98 = rtguest::allocFrame(4);
    uint32_t uStack_94 = rtguest::allocFrame(4);
    uint32_t uStack_90 = rtguest::allocFrame(4);
    uint32_t uStack_8c = rtguest::allocFrame(4);
    uint32_t uStack_88 = rtguest::allocFrame(4);
    uint32_t uStack_84 = rtguest::allocFrame(4);
uint32_t auStack_80 = rtguest::allocFrame(16);
  uint uVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  undefined4 uVar6;
  
  iVar5 = (int)param_3 + -1;
  if (param_3 != 0) {
    do {
      if ((*(int32_t*)rtguest::G((uint32_t)param_2)) == 1) {
        (*(uint64_t*)rtguest::G((uint32_t)uStack_100)) = 0;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_f8)) = 0xc100000000;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_f0)) = 0x140000007c;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
        uiArenaSetTexture(param_1,0x29f5f8,0,7);
        uiArenaDataTag(param_1,1);
        uiArenaSprites(param_1,uStack_100,1,0x102,0x14,0);
        iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(3) * sizeof(int32_t)));
        uVar4 = 0xaaa;
        if (iVar2 == -1) {
          iVar2 = 3;
        }
        else {
          uVar4 = IntToPackedBCD(iVar2);
          iVar2 = CountDecimalDigits(iVar2);
        }
        uiArenaSetTexture(param_1,0x29f5f8,0,3);
        if (0 < iVar2) {
          uVar6 = 0x42100000;
          iVar3 = iVar2 * 0xf + 0x120;
          do {
            uVar1 = uVar4 & 0xf;
            uVar4 = (int)uVar4 >> 4;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_e0)) = 0;
            iVar2 = iVar2 + -1;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_dc)) = 0;
            (*(int32_t*)rtguest::G((uint32_t)iStack_d8)) = (uVar1 % 6) * 0x28;
            (*(int32_t*)rtguest::G((uint32_t)iStack_d4)) = (uVar1 / 6) * 0x30 + 0x38;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_cc)) = 0x30;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_d0)) = 0x28;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_c8)) = 0;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_c4)) = 0;
            (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(auStack_c0)), 0, 0x10));
            (*(uint32_t*)rtguest::G((uint32_t)uStack_b4)) = 0x3f800000;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_b0)) = 0x41f00000;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = 0;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_a8)) = 0;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_a4)) = 0x3f800000;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_a0)) = 0;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_98)) = 0;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_94)) = 0x3f800000;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_90)) = 0x41f00000;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_88)) = 0;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_84)) = 0x3f800000;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_9c)) = uVar6;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_8c)) = uVar6;
            (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(auStack_80)), 0, 0x10));
            uiArenaRotQuads(param_1,uStack_e0,1,iVar3,0x28,0);
            iVar3 = iVar3 + -0x1e;
          } while (iVar2 != 0);
        }
      }
      iVar5 = iVar5 + -1;
      param_2 = param_2 + 4;
    } while (iVar5 != -1);
  }
  return 0;
}


RT_GUEST_FN(0x00231f28, guest::raceHudElementJ)

}  // namespace guest
