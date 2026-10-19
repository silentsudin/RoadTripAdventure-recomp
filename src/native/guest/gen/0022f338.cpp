// guest:0x0022f338 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t raceHudItemPanel(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
    uint32_t iStack_60 = rtguest::allocFrame(4);
    uint32_t uStack_68 = rtguest::allocFrame(8);
    uint32_t uStack_70 = rtguest::allocFrame(8);
    uint32_t uStack_78 = rtguest::allocFrame(8);
    uint32_t uStack_80 = rtguest::allocFrame(8);
  uint uVar1;
  int iVar2;
uint32_t piVar3;
  int iVar4;
  int iVar5;
uint32_t piVar6;
  int iVar7;
  int iVar8;
  
  (*(int32_t*)rtguest::G((uint32_t)iStack_60)) = (int)param_3 + -1;
  if (param_3 != 0) {
    do {
      if ((*(int32_t*)rtguest::G((uint32_t)param_2)) != 0) {
        iVar8 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t))) + -0x58;
        iVar7 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t))) + -0x2c;
        uiArenaSetTexture(param_1,0x29f5f8,0,6);
        uiArenaDataTag(param_1,0x14);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0xa800000098;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x5800000010;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
        uiArenaSprites(param_1,uStack_80,1,iVar8,iVar7,0);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0xa0;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0xa8000000b0;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x5800000010;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
        uiArenaSprites(param_1,uStack_80,1,iVar8,iVar7,0);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x10;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0xa8000000a8;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x5800000008;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
        iVar4 = 0x11;
        do {
          uiArenaSprites(param_1,uStack_80,1,iVar8,iVar7,0);
          iVar4 = iVar4 + -1;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_80 + 4)),(int)(*(uint64_t*)rtguest::G((uint32_t)uStack_80)) + (int)(*(uint64_t*)rtguest::G((uint32_t)uStack_70)));
        } while (-1 < iVar4);
        iVar5 = 0;
        piVar3 = param_2 + 0x13;
        piVar6 = param_2 + 3;
        iVar4 = (*(int32_t*)rtguest::G((uint32_t)piVar6));
        while( true ) {
          uiArenaSetTexture(param_1,0x29f5f8,0,iVar4);
          uiArenaDataTag(param_1,1);
          iVar4 = iVar5;
          if (iVar5 < 0) {
            iVar4 = iVar5 + 3;
          }
          (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0xb800000070;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = CONCAT44((iVar4 >> 2) * 0x24 + 8,(iVar5 + (iVar4 >> 2) * -4) * 0x28 + 8);
          (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x2400000028;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
          uiArenaSprites(param_1,uStack_80,1,iVar8,iVar7,0);
          uiArenaSetTexture(param_1,0x29f5f8,2,0);
          if ((*(int32_t*)rtguest::G((uint32_t)piVar6 + (uint32_t)(8) * sizeof(int32_t))) == 1) {
            if ((*(int32_t*)rtguest::G((uint32_t)piVar3)) < 0x80) {
              (*(int32_t*)rtguest::G((uint32_t)piVar3)) = (*(int32_t*)rtguest::G((uint32_t)piVar3)) + 0x40;
            }
            else {
              (*(int32_t*)rtguest::G((uint32_t)piVar3)) = 0x80;
            }
          }
          else if ((*(int32_t*)rtguest::G((uint32_t)piVar6 + (uint32_t)(8) * sizeof(int32_t))) == 2) {
            if ((*(int32_t*)rtguest::G((uint32_t)piVar3)) < 1) {
              (*(int32_t*)rtguest::G((uint32_t)piVar3)) = 0;
            }
            else {
              (*(int32_t*)rtguest::G((uint32_t)piVar3)) = (*(int32_t*)rtguest::G((uint32_t)piVar3)) + -0x20;
            }
          }
          if (iVar5 < 4) {
            uVar1 = 0x4334ec;
          }
          else {
            uVar1 = 0x50e800;
          }
          uiArenaSetColour(param_1,(*(int32_t*)rtguest::G((uint32_t)piVar3)) << 0x18 | uVar1);
          piVar3 = piVar3 + 1;
          uiArenaDataTag(param_1,1);
          iVar4 = iVar5;
          if (iVar5 < 0) {
            iVar4 = iVar5 + 3;
          }
          iVar2 = iVar5 + (iVar4 >> 2) * -4;
          iVar5 = iVar5 + 1;
          piVar6 = piVar6 + 1;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = CONCAT44((iVar4 >> 2) * 0x24 + 1,iVar2 * 0x28 + 2);
          (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x3200000032;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
          uiArenaSprites(param_1,uStack_80,1,iVar8,iVar7,0);
          if (7 < iVar5) break;
          iVar4 = (*(int32_t*)rtguest::G((uint32_t)piVar6));
        }
      }
      param_2 = param_2 + 0x1b;
      (*(int32_t*)rtguest::G((uint32_t)iStack_60)) = (*(int32_t*)rtguest::G((uint32_t)iStack_60)) + -1;
    } while ((*(int32_t*)rtguest::G((uint32_t)iStack_60)) != -1);
  }
  return 0;
}


RT_GUEST_FN(0x0022f338, guest::raceHudItemPanel)

}  // namespace guest
