// guest:0x00219660 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ApplyCollisionImpulse(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, ...) {
    uint32_t iStack_38 = rtguest::allocFrame(4);
    uint32_t iStack_3c = rtguest::allocFrame(4);
    uint32_t iStack_40 = rtguest::allocFrame(4);
    uint32_t iStack_48 = rtguest::allocFrame(4);
    uint32_t iStack_4c = rtguest::allocFrame(4);
    uint32_t iStack_50 = rtguest::allocFrame(4);
    uint32_t iStack_58 = rtguest::allocFrame(4);
    uint32_t iStack_5c = rtguest::allocFrame(4);
    uint32_t iStack_60 = rtguest::allocFrame(4);
    uint32_t iStack_78 = rtguest::allocFrame(4);
    uint32_t iStack_88 = rtguest::allocFrame(4);
    uint32_t iStack_8c = rtguest::allocFrame(4);
    uint32_t iStack_90 = rtguest::allocFrame(4);
  char cVar1;
  ushort uVar2;
  uint uVar3;
  int iVar4;
  undefined4 uVar5;
  int iVar6;
  int iVar7;
uint32_t aiStack_80 = rtguest::allocFrame(8);
uint32_t aiStack_70 = rtguest::allocFrame(16);
  
  iVar6 = (int)param_2;
  iVar7 = (int)param_1;
  uVar3 = (uint32_t)param_3;
  if (param_3 != 0) {
    (*(int32_t*)rtguest::G((uint32_t)iStack_90)) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xf0))) * -2;
    (*(int32_t*)rtguest::G((uint32_t)iStack_60)) = ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xf0))) * -0x20) / 0x19;
    (*(int32_t*)rtguest::G((uint32_t)iStack_88)) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xf8))) * -2;
    (*(int32_t*)rtguest::G((uint32_t)iStack_5c)) = ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xf4))) * -0x20) / 0x19;
    (*(int32_t*)rtguest::G((uint32_t)iStack_8c)) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xf4))) * -2;
    (*(int32_t*)rtguest::G((uint32_t)iStack_58)) = ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xf8))) * -0x20) / 0x19;
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x1d4))) = param_5;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xa0))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xa0))) + (*(int32_t*)rtguest::G((uint32_t)iStack_60)) & 0xfffffff;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xa4))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xa4))) + (*(int32_t*)rtguest::G((uint32_t)iStack_5c));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xa8))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xa8))) + (*(int32_t*)rtguest::G((uint32_t)iStack_58)) & 0xfffffff;
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x22)));
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x22))) == '\x1c') {
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x212))) == '\0') {
        if ((((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x198))) & 3) != 0) && (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) & 0x48) == 0)) {
          PlayUiSound(0x8311);
          PlayUiSound(0x311);
        }
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x212))) = 0x20;
        cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x22)));
      }
      goto LAB_002197ac;
    }
LAB_002197bc:
    TransformVecByF15Matrix(aiStack_80,iVar6 + 0xb0,iVar6 + 0xf0);
    (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) = 0;
    (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(1) * 4)) = 0;
    (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(2) * 4)) = 0;
    (*(int32_t*)rtguest::G((uint32_t)iStack_90)) = 0;
    (*(int32_t*)rtguest::G((uint32_t)iStack_8c)) = 0;
    (*(int32_t*)rtguest::G((uint32_t)iStack_88)) = 0;
    switch(uVar3 & 0xf) {
    case 1:
    case 7:
      (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) = -(*(int32_t*)rtguest::G((uint32_t)aiStack_80 + (uint32_t)(0) * 4));
      (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(2) * 4)) = -(*(int32_t*)rtguest::G((uint32_t)iStack_78));
      if ((*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) < 0) {
        (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) = (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) + 3;
      }
      if ((*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(2) * 4)) < 0) {
        (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(2) * 4)) = (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(2) * 4)) + 0xf;
      }
      (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) = (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) >> 2;
      (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(2) * 4)) = (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(2) * 4)) >> 4;
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x1d4))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x1d4))) + 0x200;
      (*(int32_t*)rtguest::G((uint32_t)iStack_88)) = -0x800;
    case 5:
      (*(int32_t*)rtguest::G((uint32_t)iStack_90)) = 0x800;
      break;
    case 2:
    case 0xb:
      (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) = -(*(int32_t*)rtguest::G((uint32_t)aiStack_80 + (uint32_t)(0) * 4));
      (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(2) * 4)) = -(*(int32_t*)rtguest::G((uint32_t)iStack_78));
      if ((*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) < 0) {
        (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) = (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) + 3;
      }
      if ((*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(2) * 4)) < 0) {
        (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(2) * 4)) = (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(2) * 4)) + 0xf;
      }
      (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) = (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) >> 2;
      (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(2) * 4)) = (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(2) * 4)) >> 4;
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x1d4))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x1d4))) + -0x200;
      (*(int32_t*)rtguest::G((uint32_t)iStack_90)) = -0x800;
      (*(int32_t*)rtguest::G((uint32_t)iStack_88)) = -0x800;
      break;
    case 3:
      (*(int32_t*)rtguest::G((uint32_t)iStack_88)) = -0x800;
      goto LAB_00219960;
    case 4:
    case 6:
    case 0xd:
      (*(int32_t*)rtguest::G((uint32_t)iStack_90)) = 0x800;
      (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(2) * 4)) = -(*(int32_t*)rtguest::G((uint32_t)iStack_78));
      if ((*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(2) * 4)) < 0) {
        (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(2) * 4)) = (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(2) * 4)) + 7;
      }
      (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) = -(*(int32_t*)rtguest::G((uint32_t)aiStack_80 + (uint32_t)(0) * 4)) / 2;
      (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(2) * 4)) = (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(2) * 4)) >> 3;
      (*(int32_t*)rtguest::G((uint32_t)iStack_88)) = 0x800;
      break;
    case 8:
    case 9:
    case 0xe:
      (*(int32_t*)rtguest::G((uint32_t)iStack_88)) = 0x800;
      (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(2) * 4)) = -(*(int32_t*)rtguest::G((uint32_t)iStack_78));
      if ((*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(2) * 4)) < 0) {
        (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(2) * 4)) = (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(2) * 4)) + 7;
      }
      (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) = -(*(int32_t*)rtguest::G((uint32_t)aiStack_80 + (uint32_t)(0) * 4)) / 2;
      (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(2) * 4)) = (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(2) * 4)) >> 3;
      (*(int32_t*)rtguest::G((uint32_t)iStack_90)) = -0x800;
      break;
    case 10:
      (*(int32_t*)rtguest::G((uint32_t)iStack_90)) = -0x800;
      break;
    case 0xc:
      (*(int32_t*)rtguest::G((uint32_t)iStack_88)) = 0x800;
LAB_00219960:
      (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(2) * 4)) = -(*(int32_t*)rtguest::G((uint32_t)iStack_78));
      break;
    case 0xf:
      if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x198))) & 3) == 0) {
        (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x198))) = 0;
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x247))) = 0xff;
      }
      else {
        ::printf((char *)rtguest::G((uint32_t)(0x333590u)));
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) | 0x200;
      }
    }
    TransformVecByF15Matrix(iStack_90,param_2,iStack_90);
    (*(int32_t*)rtguest::G((uint32_t)iStack_50)) = ((*(int32_t*)rtguest::G((uint32_t)iStack_90)) << 4) / 0x19;
    (*(int32_t*)rtguest::G((uint32_t)iStack_4c)) = ((*(int32_t*)rtguest::G((uint32_t)iStack_8c)) << 4) / 0x19;
    (*(int32_t*)rtguest::G((uint32_t)iStack_48)) = ((*(int32_t*)rtguest::G((uint32_t)iStack_88)) << 4) / 0x19;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xa0))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xa0))) + (*(int32_t*)rtguest::G((uint32_t)iStack_50)) & 0xfffffff;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xa4))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xa4))) + (*(int32_t*)rtguest::G((uint32_t)iStack_4c));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xa8))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xa8))) + (*(int32_t*)rtguest::G((uint32_t)iStack_48)) & 0xfffffff;
    TransformVecByF15Matrix(aiStack_70,param_2,aiStack_70);
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xf0))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xf0))) + (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4));
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xf8))) + (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(2) * 4));
    goto LAB_00219c08;
  }
  cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x22)));
LAB_002197ac:
  if (cVar1 != '\x1c') goto LAB_002197bc;
  TransformVecByF15Matrix(iStack_90,iVar6 + 0xb0,iVar6 + 0xf0);
  switch(uVar3 & 0xf) {
  case 0:
    break;
  case 2:
  case 0xb:
    (*(int32_t*)rtguest::G((uint32_t)iStack_90)) = -0x8e3;
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x1d4))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x1d4))) + -0x200;
    break;
  case 3:
    (*(int32_t*)rtguest::G((uint32_t)iStack_88)) = -0x11c7;
    break;
  case 5:
    (*(int32_t*)rtguest::G((uint32_t)iStack_90)) = 0x11c7;
    goto LAB_00219b40;
  case 7:
    (*(int32_t*)rtguest::G((uint32_t)iStack_88)) = -0x11c7;
  case 1:
    (*(int32_t*)rtguest::G((uint32_t)iStack_90)) = 0x8e3;
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x1d4))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x1d4))) + 0x200;
    break;
  case 8:
  case 9:
  case 10:
  case 0xe:
    (*(int32_t*)rtguest::G((uint32_t)iStack_90)) = -0x11c7;
LAB_00219b40:
    (*(int32_t*)rtguest::G((uint32_t)iStack_88)) = (*(int32_t*)rtguest::G((uint32_t)iStack_88)) / 2;
    break;
  case 0xc:
    (*(int32_t*)rtguest::G((uint32_t)iStack_88)) = 0x11c7;
    break;
  case 0xd:
    (*(int32_t*)rtguest::G((uint32_t)iStack_88)) = 0x11c7;
  case 4:
  case 6:
    (*(int32_t*)rtguest::G((uint32_t)iStack_90)) = 0x11c7;
  case 0xf:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) | 0x200;
    break;
  default:
    goto switchD_00219aac_default;
  }
switchD_00219aac_default:
  TransformVecByF15Matrix(iStack_90,param_2,iStack_90);
  (*(int32_t*)rtguest::G((uint32_t)iStack_40)) = ((*(int32_t*)rtguest::G((uint32_t)iStack_90)) << 4) / 0x19;
  (*(int32_t*)rtguest::G((uint32_t)iStack_3c)) = ((*(int32_t*)rtguest::G((uint32_t)iStack_8c)) << 4) / 0x19;
  (*(int32_t*)rtguest::G((uint32_t)iStack_38)) = ((*(int32_t*)rtguest::G((uint32_t)iStack_88)) << 4) / 0x19;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xa0))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xa0))) + (*(int32_t*)rtguest::G((uint32_t)iStack_40)) & 0xfffffff;
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xa4))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xa4))) + (*(int32_t*)rtguest::G((uint32_t)iStack_3c));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xa8))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xa8))) + (*(int32_t*)rtguest::G((uint32_t)iStack_38)) & 0xfffffff;
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xf0))) = (*(int32_t*)rtguest::G((uint32_t)iStack_90));
  iVar4 = (*(int32_t*)rtguest::G((uint32_t)iStack_88));
LAB_00219c08:
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xf8))) = iVar4;
  uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x198)));
  if (((uVar2 & 0xfb0) == 0) && (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) & 0x40) == 0)) {
    switch(uVar3 & 0xf) {
    case 0:
      return 0;
    case 1:
    case 4:
    case 5:
      uVar5 = 4;
      break;
    case 2:
    case 8:
    case 10:
      uVar5 = 3;
      break;
    case 3:
    case 6:
    case 7:
    case 9:
    default:
      gs_controls_rumble_start(param_1,uVar2 & 1,5,0xff);
    }
    gs_controls_rumble_start(param_1,uVar2 & 1,uVar5,0xff);
  }
}


RT_GUEST_FN(0x00219660, guest::ApplyCollisionImpulse)

}  // namespace guest
