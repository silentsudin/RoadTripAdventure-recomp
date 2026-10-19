// guest:0x0022f750 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t raceHudElementC(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
    uint32_t uStack_58 = rtguest::allocFrame(8);
    uint32_t uStack_60 = rtguest::allocFrame(8);
    uint32_t uStack_68 = rtguest::allocFrame(8);
    uint32_t uStack_70 = rtguest::allocFrame(8);
  int iVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  
  iVar6 = (int)param_3 + -1;
  if (param_3 != 0) {
    do {
      if ((*(int32_t*)rtguest::G((uint32_t)param_2)) != 0) {
        iVar4 = 0xb;
        uiArenaSetTexture(param_1,0x29f5f8,0,1);
        uiArenaDataTag(param_1,0xe);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0xb000000000;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = 0x1800000012;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_58)) = 0;
        uiArenaSprites(param_1,uStack_70,1,(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t))),(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t))),0);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x72;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0xb00000001a;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = 0x1800000012;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_58)) = 0;
        uiArenaSprites(param_1,uStack_70,1,(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t))),(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t))),0);
        (*(uint32_t*)rtguest::G((uint32_t)uStack_70 + 0)) = 0x12;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_70 + 4)) = 0;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0xb000000012;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = 0x1800000008;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_58)) = 0;
        do {
          uiArenaSprites(param_1,uStack_70,1,(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t))),(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t))),0);
          iVar4 = iVar4 + -1;
          (*(uint32_t*)rtguest::G((uint32_t)uStack_70 + 0)) = (int)(*(uint64_t*)rtguest::G((uint32_t)uStack_70)) + (int)(*(uint64_t*)rtguest::G((uint32_t)uStack_60));
        } while (-1 < iVar4);
        iVar4 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(4) * sizeof(int32_t)));
        if ((iVar4 == 0) || (iVar4 == 0x7fffffff)) {
          uVar3 = 0xaaaaaa;
        }
        else if (iVar4 < 360000) {
          iVar1 = (iVar4 / 0x3c) % 0x3c;
          uVar3 = (((((iVar4 / 36000) * 0x10 + (iVar4 / 0xe10) % 10) * 0x10 + iVar1 / 10) * 0x10 +
                   iVar1 % 10) * 0x10 + (iVar4 % 0x3c) / 6) * 0x10 +
                  (((iVar4 % 0x3c) * 100) / 0x3c) % 10;
        }
        else {
          uVar3 = 0x995999;
        }
        uVar5 = 0;
        uiArenaSetTexture(param_1,0x29f5f8,0,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0029fca8 + (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(3) * sizeof(int32_t))) * 4))));
        uiArenaDataTag(param_1,8);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x400000020;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0x30000000a0;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = 0x1000000010;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_58)) = 0;
        uiArenaSprites(param_1,uStack_70,1,(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t))),(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t))),0);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x40000004a;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0x30000000b0;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = 0x1000000010;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_58)) = 0;
        uiArenaSprites(param_1,uStack_70,1,(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t))),(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t))),0);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x400000066;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = 0x1000000010;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_58)) = 0;
        do {
          uVar2 = uVar3 & 0xf;
          uVar3 = (int)uVar3 >> 4;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = CONCAT44(0x40,uVar2 << 4);
          uiArenaSprites(param_1,uStack_70,1,(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t))),(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t))),0);
          uVar2 = uVar5 & 1;
          uVar5 = uVar5 + 1;
          if (uVar2 == 0) {
            iVar4 = ((int)(*(uint64_t*)rtguest::G((uint32_t)uStack_70)) - (int)(*(uint64_t*)rtguest::G((uint32_t)uStack_60))) + 2;
          }
          else {
            iVar4 = (int)(*(uint64_t*)rtguest::G((uint32_t)uStack_70)) + (int)(*(uint64_t*)rtguest::G((uint32_t)uStack_60)) * -2 + 4;
          }
          (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_70 + 4)),iVar4);
        } while ((int)uVar5 < 6);
        iVar4 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(5) * sizeof(int32_t)));
        if (iVar4 != -1) {
          uiArenaSetTexture(param_1,0x29f5f8,1,8);
          uiArenaDataTag(param_1,1);
          (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0xffffffe6;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0xa8000000e8;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = 0x1800000018;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_58)) = 0;
          uiArenaSprites(param_1,uStack_70,1,(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t))),(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t))),0);
          uiArenaSetTexture(param_1,0x29f5f8,1,7);
          if (iVar4 == 0) {
            uiArenaDataTag(param_1,1);
            (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x5ffffffe2;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0xc800000050;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = 0x1000000020;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_58)) = 0;
            uiArenaSprites(param_1,uStack_70,1,(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t))),(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t))),0);
          }
          else if (iVar4 < 10) {
            uiArenaDataTag(param_1,1);
            (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x5ffffffee;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = CONCAT44(200,iVar4 << 3);
            (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = 0x1000000008;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_58)) = 0;
            uiArenaSprites(param_1,uStack_70,1,(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t))),(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t))),0);
          }
          else {
            uiArenaDataTag(param_1,2);
            (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x5ffffffea;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = 0x1000000008;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_58)) = 0;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = CONCAT44(200,iVar4 / 10 << 3);
            uiArenaSprites(param_1,uStack_70,1,(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t))),(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t))),0);
            (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x5fffffff2;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = CONCAT44(200,iVar4 % 10 << 3);
            (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = 0x1000000008;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_58)) = 0;
            uiArenaSprites(param_1,uStack_70,1,(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t))),(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t))),0);
          }
        }
      }
      iVar6 = iVar6 + -1;
      param_2 = param_2 + 6;
    } while (iVar6 != -1);
  }
  return 0;
}


RT_GUEST_FN(0x0022f750, guest::raceHudElementC)

}  // namespace guest
