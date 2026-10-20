// guest:0x00221f70 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawCarShowroom(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(384);
uint32_t auStack_180 = rtguest::allocFrame(52);
    uint32_t fStack_14c = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
uint32_t auStack_140 = rtguest::allocFrame(48);
uint32_t auStack_110 = rtguest::allocFrame(16);
    uint32_t uStack_100 = rtguest::allocFrame(4);
    uint32_t uStack_fc = rtguest::allocFrame(4);
    uint32_t uStack_f8 = rtguest::allocFrame(4);
    uint32_t uStack_f4 = rtguest::allocFrame(4);
uint32_t auStack_f0 = rtguest::allocFrame(64);
uint32_t auStack_b0 = rtguest::allocFrame(48);
uint32_t auStack_80 = rtguest::allocFrame(16);
    uint32_t uStack_70 = rtguest::allocFrame(4);
  ushort uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
uint32_t puVar5;
  uint uVar6;
  int iVar7;
  
  iVar7 = (int)param_2;
  iVar2 = (*(uint32_t*)rtguest::G((uint32_t)0x177ea50 + (uint32_t)(param_3) * 4));
  iVar3 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) * 0x44;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_70)) = param_1;
  vu0MulMatrix(auStack_180,param_2,iVar7 + 0x40);
  (*(float*)rtguest::G((uint32_t)fStack_14c)) = (*(float*)rtguest::G((uint32_t)fStack_14c)) - 1.0;
  vu0MulMatrix(auStack_180,(*(uint32_t*)rtguest::G((uint32_t)uStack_70)),auStack_180);
  (*(float*)rtguest::G((uint32_t)fStack_14c)) = (*(float*)rtguest::G((uint32_t)fStack_14c)) + 1.0;
  vu0CopyMatrix(auStack_140,auStack_180);
  vifUploadAt16(0x177f5e0);
  vifUploadLightMatrix(0x29d1d0);
  vifObjectMatrix(auStack_180);
  vifUploadPacket(0x29d210,3);
  vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x177f310 + (uint32_t)(param_3 * 6) * 4)) + 0x10);
  if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 8))) & 0x1000) == 0) {
    vifCallModel((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177f318 + param_3 * 0x18))) + 0x10);
    uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 8)));
  }
  else {
    vifCallModel((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177f31c + param_3 * 0x18))) + 0x10);
    uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 8)));
  }
  if ((uVar1 & 0x2000) != 0) {
    vifCallModel((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177f320 + param_3 * 0x18))) + 0x10);
  }
  uVar6 = 0;
  uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 8)));
  do {
    if (((int)(uint32_t)uVar1 >> (uVar6 & 0x1f) & 1U) != 0) {
      (*(uint32_t*)rtguest::G((uint32_t)uStack_100)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_f4)) = 0x3f800000;
      if (uVar6 < 10) {
        iVar4 = uVar6 * 4;
        switch(uVar6) {
        default:
          (*(uint32_t*)rtguest::G((uint32_t)uStack_fc)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed728 + iVar3)));
          (*(uint32_t*)rtguest::G((uint32_t)uStack_f8)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed72c + iVar3)));
          break;
        case 2:
        case 3:
          (*(uint32_t*)rtguest::G((uint32_t)uStack_fc)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed730 + iVar3)));
          (*(uint32_t*)rtguest::G((uint32_t)uStack_f8)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed734 + iVar3)));
          break;
        case 4:
          (*(uint32_t*)rtguest::G((uint32_t)uStack_fc)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed738 + iVar3)));
          (*(uint32_t*)rtguest::G((uint32_t)uStack_f8)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed73c + iVar3)));
          break;
        case 5:
        case 9:
          (*(uint32_t*)rtguest::G((uint32_t)uStack_fc)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed740 + iVar3)));
          (*(uint32_t*)rtguest::G((uint32_t)uStack_f8)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed744 + iVar3)));
          break;
        case 6:
          (*(uint32_t*)rtguest::G((uint32_t)uStack_fc)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed748 + iVar3)));
          (*(uint32_t*)rtguest::G((uint32_t)uStack_f8)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed74c + iVar3)));
          break;
        case 7:
          break;
        case 8:
          (*(uint32_t*)rtguest::G((uint32_t)uStack_fc)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed750 + iVar3)));
          (*(uint32_t*)rtguest::G((uint32_t)uStack_f8)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed754 + iVar3)));
        }
      }
      else {
        iVar4 = uVar6 << 2;
      }
      vu0ApplyMatrix(auStack_110,auStack_180,uStack_100);
      vifObjectMatrix(auStack_140);
      vifCallModel((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177eac8 + iVar4))) + 0x10);
    }
    uVar6 = uVar6 + 1;
    if (9 < (int)uVar6) {
      vu0CopyMatrix(auStack_180,param_2);
      (*(float*)rtguest::G((uint32_t)fStack_14c)) = (*(float*)rtguest::G((uint32_t)fStack_14c)) - 1.0;
      vu0MulMatrix(auStack_180,(*(uint32_t*)rtguest::G((uint32_t)uStack_70)),auStack_180);
      (*(float*)rtguest::G((uint32_t)fStack_14c)) = (*(float*)rtguest::G((uint32_t)fStack_14c)) + 1.0;
      if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 8))) & 0x400) == 0) {
        drawCarWheelsMid(auStack_180,iVar7 + 0x80,iVar2);
      }
      else {
        vifFixedRegs(0x20138c85dd309c44,7,(uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 5))) << 0x38 | 0x138c85dd409c44,7);
        uVar6 = 0;
        vu0UnitMatrix(auStack_b0);
        vu0RotMatrixX((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x80))),auStack_b0,auStack_b0);
        vu0RotMatrixY((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x84))),auStack_b0,auStack_b0);
        puVar5 = 0x0029cb20;
        do {
          vu0CopyVector(auStack_80,puVar5);
          vu0MulMatrix(auStack_f0,auStack_180,auStack_b0);
          vifObjectMatrix(auStack_f0);
          vifCallModel((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177eb08 + (uVar6 & 1) * 4))) + 0x10);
          if (uVar6 == 1) {
            vu0UnitMatrix(auStack_b0);
            vu0RotMatrixX((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x88))),auStack_b0,auStack_b0);
          }
          uVar6 = uVar6 + 1;
          puVar5 = puVar5 + 0x10;
        } while ((int)uVar6 < 4);
      }
      return 0;
    }
    uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 8)));
  } while( true );
}


RT_GUEST_FN(0x00221f70, guest::drawCarShowroom)

}  // namespace guest
