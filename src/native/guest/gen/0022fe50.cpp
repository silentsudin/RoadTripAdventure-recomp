// guest:0x0022fe50 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t raceHudElementD(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(112);
    uint32_t uStack_70 = rtguest::allocFrame(8);
    uint32_t uStack_68 = rtguest::allocFrame(8);
    uint32_t uStack_60 = rtguest::allocFrame(8);
    uint32_t uStack_58 = rtguest::allocFrame(8);
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  iVar4 = (int)param_3 + -1;
  if (param_3 != 0) {
    do {
      if ((*(int32_t*)rtguest::G((uint32_t)param_2)) != 0) {
        if ((*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(4) * sizeof(int32_t))) == 0) {
          if ((*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(3) * sizeof(int32_t))) != -1) {
            uiArenaSetTexture(param_1,0x29f5f8,0,0);
            uiArenaDataTag(param_1,1);
            iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(3) * sizeof(int32_t))) % 10;
            iVar2 = 3;
            if ((iVar3 < 3) && (iVar2 = iVar3, (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(3) * sizeof(int32_t))) / 10 == 1)) {
              iVar2 = 3;
            }
            (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = CONCAT44(0x1f,iVar2 * 0x16 + 0x20);
            (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1e;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = 0x1000000016;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_58)) = 0;
            uiArenaSprites(param_1,uStack_70,1,(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t))),(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t))),0);
            uiArenaSetTexture(param_1,0x29f5f8,0,0);
            iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(3) * sizeof(int32_t))) + 1;
            if (iVar3 < 10) {
              uiArenaDataTag(param_1,1);
              iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t)));
              iVar1 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t)));
              (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0xe;
              (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = CONCAT44(0x40,iVar3 * 0x10);
              (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = 0x1000000010;
              goto LAB_00230418;
            }
            uiArenaDataTag(param_1,2);
            (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0xe;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = 0x1000000010;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_58)) = 0;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = CONCAT44(0x40,iVar3 % 10 << 4);
            uiArenaSprites(param_1,uStack_70,1,(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t))),(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t))),0);
            (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = CONCAT44(0x40,iVar3 / 10 << 4);
            (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = 0x1000000010;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_58)) = 0;
            uiArenaSprites(param_1,uStack_70,1,(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t))),(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t))),0);
          }
        }
        else {
          uiArenaSetTexture(param_1,0x29f5f8,0,0);
          uiArenaDataTag(param_1,2);
          (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x33;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0x69000000a0;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = 0x2000000020;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_58)) = 0;
          uiArenaSprites(param_1,uStack_70,1,(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t))),(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t))),0);
          iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(3) * sizeof(int32_t))) % 10;
          iVar2 = 3;
          if ((iVar3 < 3) && (iVar2 = iVar3, (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(3) * sizeof(int32_t))) / 10 == 1)) {
            iVar2 = 3;
          }
          (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = CONCAT44(0x1f,iVar2 * 0x16 + 0x20);
          (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x33;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = 0x1000000016;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_58)) = 0;
          uiArenaSprites(param_1,uStack_70,1,(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t))),(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t))),0);
          uiArenaSetTexture(param_1,0x29f5f8,0,0);
          iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(3) * sizeof(int32_t))) + 1;
          if (iVar3 < 10) {
            uiArenaDataTag(param_1,1);
            (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x17;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = CONCAT44((iVar3 / 9) * 0x1e,(iVar3 % 9) * 0x1c);
            (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = 0x1e0000001c;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_58)) = 0;
            uiArenaSprites(param_1,uStack_70,1,(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t))),(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t))),0);
          }
          else {
            uiArenaDataTag(param_1,2);
            (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x17;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = CONCAT44(((iVar3 % 10) / 9) * 0x1e,((iVar3 % 10) % 9) * 0x1c);
            (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = 0x1e0000001c;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_58)) = 0;
            uiArenaSprites(param_1,uStack_70,1,(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t))),(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t))),0);
            (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_58)) = 0;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = CONCAT44((iVar3 / 0x5a) * 0x1e,((iVar3 / 10) % 9) * 0x1c);
            (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = 0x1e0000001c;
            uiArenaSprites(param_1,uStack_70,1,(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t))),(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t))),0);
          }
          uiArenaSetTexture(param_1,0x29f5f8,0,1);
          iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(4) * sizeof(int32_t)));
          if (iVar3 < 10) {
            uiArenaDataTag(param_1,1);
            iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t)));
            iVar1 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t)));
            (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1000000047;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = CONCAT44(0x30,iVar3 * 0xc + 0x20);
            (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = 0x100000000c;
          }
          else {
            uiArenaDataTag(param_1,2);
            (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x1000000047;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = 0x100000000c;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_58)) = 0;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = CONCAT44(0x30,(iVar3 % 10) * 0xc + 0x20);
            uiArenaSprites(param_1,uStack_70,1,(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t))),(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t))),0);
            (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x100000003d;
            iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t)));
            iVar1 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t)));
            (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = CONCAT44(0x30,(iVar3 / 10) * 0xc + 0x20);
            (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = 0x100000000c;
          }
LAB_00230418:
          (*(uint64_t*)rtguest::G((uint32_t)uStack_58)) = 0;
          uiArenaSprites(param_1,uStack_70,1,iVar2,iVar1,0);
        }
      }
      iVar4 = iVar4 + -1;
      param_2 = param_2 + 5;
    } while (iVar4 != -1);
  }
  return 0;
}


RT_GUEST_FN(0x0022fe50, guest::raceHudElementD)

}  // namespace guest
