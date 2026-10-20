// guest:0x0022ee70 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t raceHudElementA(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
    uint32_t uStack_b0 = rtguest::allocFrame(4);
    uint32_t uStack_ac = rtguest::allocFrame(4);
    uint32_t uStack_a8 = rtguest::allocFrame(4);
    uint32_t uStack_a4 = rtguest::allocFrame(4);
    uint32_t uStack_a0 = rtguest::allocFrame(4);
    uint32_t uStack_9c = rtguest::allocFrame(4);
    uint32_t uStack_98 = rtguest::allocFrame(4);
    uint32_t uStack_94 = rtguest::allocFrame(4);
    uint32_t fStack_90 = rtguest::allocFrame(4);
    uint32_t fStack_8c = rtguest::allocFrame(4);
    uint32_t uStack_88 = rtguest::allocFrame(4);
    uint32_t uStack_84 = rtguest::allocFrame(4);
    uint32_t fStack_80 = rtguest::allocFrame(4);
    uint32_t fStack_7c = rtguest::allocFrame(4);
    uint32_t uStack_78 = rtguest::allocFrame(4);
    uint32_t uStack_74 = rtguest::allocFrame(4);
    uint32_t fStack_70 = rtguest::allocFrame(4);
    uint32_t fStack_6c = rtguest::allocFrame(4);
    uint32_t uStack_68 = rtguest::allocFrame(4);
    uint32_t uStack_64 = rtguest::allocFrame(4);
    uint32_t fStack_60 = rtguest::allocFrame(4);
    uint32_t fStack_5c = rtguest::allocFrame(4);
    uint32_t uStack_58 = rtguest::allocFrame(4);
    uint32_t uStack_54 = rtguest::allocFrame(4);
    uint32_t uStack_50 = rtguest::allocFrame(4);
    uint32_t uStack_4c = rtguest::allocFrame(4);
    uint32_t fStack_48 = rtguest::allocFrame(4);
    uint32_t uStack_44 = rtguest::allocFrame(4);
    uint32_t iStack_40 = rtguest::allocFrame(4);
    uint32_t iStack_3c = rtguest::allocFrame(4);
uint32_t piVar1;
uint32_t piVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  
  iVar7 = (int)param_3 + -1;
  if (param_3 != 0) {
    do {
      iVar4 = (*(int32_t*)rtguest::G((uint32_t)param_2));
      if (iVar4 == 1) {
        iVar4 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(6) * sizeof(int32_t)));
        iVar5 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(5) * sizeof(int32_t)));
        iVar6 = 0;
        iVar3 = 0;
        while (iVar4 = iVar4 + -1, iVar4 != -1) {
          piVar1 = (uint32_t)(iVar5 + 0x10);
          piVar2 = (uint32_t)(iVar5 + 0x14);
          iVar5 = iVar5 + 0x20;
          if (iVar6 < (*(int32_t*)rtguest::G((uint32_t)piVar1))) {
            iVar6 = (*(int32_t*)rtguest::G((uint32_t)piVar1));
          }
          if (iVar3 < (*(int32_t*)rtguest::G((uint32_t)piVar2))) {
            iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar2));
          }
        }
        (*(int32_t*)rtguest::G((uint32_t)iStack_40)) = -iVar6 / 2 + (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t)));
        (*(int32_t*)rtguest::G((uint32_t)iStack_3c)) = -iVar3 / 2 + (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t)));
        uiArenaSetTexture(param_1,0x29f5f8,(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(3) * sizeof(int32_t))),(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(4) * sizeof(int32_t))));
        uiArenaDataTag(param_1,(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(6) * sizeof(int32_t))));
        uiArenaSprites(param_1,(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(5) * sizeof(int32_t))),(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(6) * sizeof(int32_t))),(*(int32_t*)rtguest::G((uint32_t)iStack_40)),(*(int32_t*)rtguest::G((uint32_t)iStack_3c)),0);
      }
      else if (1 < iVar4) {
        if (iVar4 == 2) {
          (*(float*)rtguest::G((uint32_t)fStack_6c)) = (float)(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(10) * sizeof(int32_t)));
          (*(int32_t*)rtguest::G((uint32_t)iStack_40)) = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t)));
          (*(int32_t*)rtguest::G((uint32_t)iStack_3c)) = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t)));
          if ((*(float*)rtguest::G((uint32_t)fStack_6c)) == 0.0) {
            (*(float*)rtguest::G((uint32_t)fStack_6c)) = 1.0;
          }
          iVar4 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(5) * sizeof(int32_t)));
          (*(uint32_t*)rtguest::G((uint32_t)uStack_88)) = 0;
          (*(uint32_t*)rtguest::G((uint32_t)uStack_b0)) = 0;
          (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = 0;
          (*(uint32_t*)rtguest::G((uint32_t)uStack_a8)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 8)));
          (*(uint32_t*)rtguest::G((uint32_t)uStack_84)) = 0x3f800000;
          (*(uint32_t*)rtguest::G((uint32_t)uStack_a4)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xc)));
          (*(float*)rtguest::G((uint32_t)fStack_80)) = (*(float*)rtguest::G((uint32_t)fStack_6c)) * (float)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x10))) / 2);
          (*(uint32_t*)rtguest::G((uint32_t)uStack_78)) = 0;
          (*(float*)rtguest::G((uint32_t)fStack_6c)) = (*(float*)rtguest::G((uint32_t)fStack_6c)) * (float)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x14))) / 2);
          (*(uint32_t*)rtguest::G((uint32_t)uStack_74)) = 0x3f800000;
          (*(uint32_t*)rtguest::G((uint32_t)uStack_a0)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x10)));
          (*(float*)rtguest::G((uint32_t)fStack_90)) = -(*(float*)rtguest::G((uint32_t)fStack_80));
          (*(float*)rtguest::G((uint32_t)fStack_8c)) = -(*(float*)rtguest::G((uint32_t)fStack_6c));
          (*(uint32_t*)rtguest::G((uint32_t)uStack_9c)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x14)));
          (*(uint32_t*)rtguest::G((uint32_t)uStack_98)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x18)));
          (*(uint32_t*)rtguest::G((uint32_t)uStack_94)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c)));
          (*(uint32_t*)rtguest::G((uint32_t)uStack_68)) = 0;
          (*(uint32_t*)rtguest::G((uint32_t)uStack_64)) = 0x3f800000;
          (*(uint32_t*)rtguest::G((uint32_t)uStack_58)) = 0;
          (*(uint32_t*)rtguest::G((uint32_t)uStack_54)) = 0x3f800000;
          (*(uint32_t*)rtguest::G((uint32_t)uStack_50)) = 0;
          (*(float*)rtguest::G((uint32_t)fStack_48)) = (float)(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(7) * sizeof(int32_t)));
          (*(uint32_t*)rtguest::G((uint32_t)uStack_4c)) = 0;
          (*(uint32_t*)rtguest::G((uint32_t)uStack_44)) = 0;
          (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(7) * sizeof(int32_t))) = ((*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(7) * sizeof(int32_t))) + 1) % 0x3c;
          if (45.0 <= (*(float*)rtguest::G((uint32_t)fStack_48))) {
            (*(float*)rtguest::G((uint32_t)fStack_48)) = (*(float*)rtguest::G((uint32_t)fStack_48)) - 60.0;
          }
          else if (15.0 <= (*(float*)rtguest::G((uint32_t)fStack_48))) {
            (*(float*)rtguest::G((uint32_t)fStack_48)) = 30.0 - (*(float*)rtguest::G((uint32_t)fStack_48));
          }
          (*(float*)rtguest::G((uint32_t)fStack_48)) = (*(float*)rtguest::G((uint32_t)fStack_48)) * (*(float*)rtguest::G(0x332e34));
          (*(float*)rtguest::G((uint32_t)fStack_7c)) = (*(float*)rtguest::G((uint32_t)fStack_8c));
          (*(float*)rtguest::G((uint32_t)fStack_70)) = (*(float*)rtguest::G((uint32_t)fStack_90));
          (*(float*)rtguest::G((uint32_t)fStack_60)) = (*(float*)rtguest::G((uint32_t)fStack_80));
          (*(float*)rtguest::G((uint32_t)fStack_5c)) = (*(float*)rtguest::G((uint32_t)fStack_6c));
          uiArenaSetTexture(param_1,0x29f5f8,(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(3) * sizeof(int32_t))),(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(4) * sizeof(int32_t))));
          uiArenaRotQuads(param_1,uStack_b0,1,(*(int32_t*)rtguest::G((uint32_t)iStack_40)),(*(int32_t*)rtguest::G((uint32_t)iStack_3c)),0);
        }
        else if (iVar4 == 3) {
          (*(int32_t*)rtguest::G((uint32_t)iStack_40)) = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t)));
          (*(int32_t*)rtguest::G((uint32_t)iStack_3c)) = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t)));
          iVar4 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(9) * sizeof(int32_t)));
          if (iVar4 == 0) {
            (*(float*)rtguest::G((uint32_t)fStack_6c)) = (float)(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(10) * sizeof(int32_t)));
            if ((*(float*)rtguest::G((uint32_t)fStack_6c)) == 0.0) {
              (*(float*)rtguest::G((uint32_t)fStack_6c)) = 1.0;
              goto LAB_0022f200;
            }
            iVar4 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(5) * sizeof(int32_t)));
          }
          else {
            iVar5 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(7) * sizeof(int32_t)));
            (*(float*)rtguest::G((uint32_t)fStack_6c)) = ((float)(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(10) * sizeof(int32_t))) * (float)iVar5) / (float)iVar4;
            if (iVar5 < iVar4) {
              (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(7) * sizeof(int32_t))) = iVar5 + 1;
LAB_0022f200:
              iVar4 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(5) * sizeof(int32_t)));
            }
            else {
              iVar4 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(5) * sizeof(int32_t)));
            }
          }
          (*(uint32_t*)rtguest::G((uint32_t)uStack_88)) = 0;
          (*(uint32_t*)rtguest::G((uint32_t)uStack_b0)) = 0;
          (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = 0;
          (*(uint32_t*)rtguest::G((uint32_t)uStack_a8)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 8)));
          (*(uint32_t*)rtguest::G((uint32_t)uStack_84)) = 0x3f800000;
          (*(uint32_t*)rtguest::G((uint32_t)uStack_a4)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xc)));
          (*(float*)rtguest::G((uint32_t)fStack_80)) = (*(float*)rtguest::G((uint32_t)fStack_6c)) * (float)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x10))) / 2);
          (*(uint32_t*)rtguest::G((uint32_t)uStack_78)) = 0;
          (*(float*)rtguest::G((uint32_t)fStack_6c)) = (*(float*)rtguest::G((uint32_t)fStack_6c)) * (float)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x14))) / 2);
          (*(uint32_t*)rtguest::G((uint32_t)uStack_74)) = 0x3f800000;
          (*(uint32_t*)rtguest::G((uint32_t)uStack_a0)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x10)));
          (*(float*)rtguest::G((uint32_t)fStack_90)) = -(*(float*)rtguest::G((uint32_t)fStack_80));
          (*(float*)rtguest::G((uint32_t)fStack_8c)) = -(*(float*)rtguest::G((uint32_t)fStack_6c));
          (*(uint32_t*)rtguest::G((uint32_t)uStack_9c)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x14)));
          (*(uint32_t*)rtguest::G((uint32_t)uStack_98)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x18)));
          (*(uint32_t*)rtguest::G((uint32_t)uStack_94)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c)));
          (*(uint32_t*)rtguest::G((uint32_t)uStack_68)) = 0;
          (*(uint32_t*)rtguest::G((uint32_t)uStack_64)) = 0x3f800000;
          (*(uint32_t*)rtguest::G((uint32_t)uStack_58)) = 0;
          (*(uint32_t*)rtguest::G((uint32_t)uStack_54)) = 0x3f800000;
          (*(uint32_t*)rtguest::G((uint32_t)uStack_50)) = 0;
          (*(uint32_t*)rtguest::G((uint32_t)uStack_4c)) = 0;
          (*(float*)rtguest::G((uint32_t)fStack_48)) = 0.0;
          (*(uint32_t*)rtguest::G((uint32_t)uStack_44)) = 0;
          (*(float*)rtguest::G((uint32_t)fStack_7c)) = (*(float*)rtguest::G((uint32_t)fStack_8c));
          (*(float*)rtguest::G((uint32_t)fStack_70)) = (*(float*)rtguest::G((uint32_t)fStack_90));
          (*(float*)rtguest::G((uint32_t)fStack_60)) = (*(float*)rtguest::G((uint32_t)fStack_80));
          (*(float*)rtguest::G((uint32_t)fStack_5c)) = (*(float*)rtguest::G((uint32_t)fStack_6c));
          uiArenaSetTexture(param_1,0x29f5f8,(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(3) * sizeof(int32_t))),(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(4) * sizeof(int32_t))));
          uiArenaRotQuads(param_1,uStack_b0,1,(*(int32_t*)rtguest::G((uint32_t)iStack_40)),(*(int32_t*)rtguest::G((uint32_t)iStack_3c)),0);
        }
      }
      iVar7 = iVar7 + -1;
      param_2 = param_2 + 0xb;
    } while (iVar7 != -1);
  }
  return 0;
}


RT_GUEST_FN(0x0022ee70, guest::raceHudElementA)

}  // namespace guest
