// guest:0x002318b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t raceHudElementH(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
    uint32_t iStack_60 = rtguest::allocFrame(4);
    uint32_t uStack_68 = rtguest::allocFrame(8);
    uint32_t uStack_70 = rtguest::allocFrame(8);
    uint32_t uStack_78 = rtguest::allocFrame(8);
    uint32_t uStack_80 = rtguest::allocFrame(8);
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
        uiArenaSetTexture(param_1,0x29f5f8,1,2);
        uiArenaDataTag(param_1,1);
        uiArenaSprites(param_1,uStack_80,1,0,0,0);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x4100000062;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0xc2000000a0;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1400000018;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
        uiArenaSetTexture(param_1,0x29f5f8,1,1);
        uiArenaDataTag(param_1,1);
        uiArenaSprites(param_1,uStack_80,1,0,0,0);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x1800000232;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0x68000000c8;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x3000000030;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
        uiArenaSetTexture(param_1,0x29f5f8,1,4);
        uiArenaDataTag(param_1,1);
        uiArenaSprites(param_1,uStack_80,1,0,0,0);
        uVar1 = (*(uint32_t*)rtguest::G(0x1780a28));
        uVar2 = IntToPackedBCD((*(uint32_t*)rtguest::G(0x1780a28)));
        iVar3 = CountDecimalDigits(uVar1);
        uiArenaSetTexture(param_1,0x29f5f8,1,1);
        uiArenaDataTag(param_1,iVar3);
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
            uiArenaSprites(param_1,uStack_80,1,0,0,0);
            iVar5 = iVar5 + -0xf;
          } while (iVar3 != 0);
          iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(3) * sizeof(int32_t)));
        }
        if (iVar3 < 0) {
          iVar3 = 0;
        }
        uVar2 = IntToPackedBCD(iVar3);
        iVar3 = CountDecimalDigits(iVar3);
        uiArenaSetTexture(param_1,0x29f5f8,1,4);
        uiArenaDataTag(param_1,iVar3);
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
            uiArenaSprites(param_1,uStack_80,1,0,0,0);
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
