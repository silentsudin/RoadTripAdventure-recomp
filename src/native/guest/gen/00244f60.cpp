// guest:0x00244f60 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawCarouselCar(uint32_t param_1, uint32_t param_2, ...) {
  undefined8 uVar1;
  ulong uVar2;
  int iVar3;
  undefined8 uVar4;
uint32_t psVar5;
  uint uVar6;
  int iVar7;
uint32_t auStack_70 = rtguest::allocFrame(48);
uint32_t auStack_40 = rtguest::allocFrame(16);
  
  iVar7 = param_2 * 2;
  psVar5 = (uint32_t)(param_1 + iVar7 + 0x5420);
  vifDirect(param_1 + 0x60,2);
  vifDirect(param_1 + param_2 * 0xd0 + 0x80,0xd);
  uVar1 = DispatchByIndex16((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5400))),(*(int16_t*)rtguest::G((uint32_t)psVar5)));
  iVar3 = param_2 * 8 + param_1;
  uVar4 = 5;
  if ((long)(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5404))) != (long)(*(int16_t*)rtguest::G((uint32_t)psVar5))) {
    uVar4 = 0;
  }
  uiDrawTextScaled((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x5380))),(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x5382))),
                   (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x5384))),uVar1,uVar4,(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x5386))));
  vu0UnitMatrix(auStack_70);
  switch((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5400)))) {
  case 0:
    uVar6 = param_2 & 7;
    psVar5 = (uint32_t)(param_1 + iVar7 + 0x5420);
    uVar2 = (uint32_t)(*(int16_t*)rtguest::G((uint32_t)psVar5));
    if ((uint32_t)(*(uint8_t*)rtguest::G((uint32_t)0x335720 + (uint32_t)(uVar6) * 1)) == uVar2) {
      vifDirect(param_1 + uVar6 * 0xf0 + 0x2b80,2);
      vifUploadAt16(0x177f5e0);
      UploadPackedColors(0x017804d0 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x540c))) * 0xd12 + (int)(*(int16_t*)rtguest::G((uint32_t)psVar5)));
      vifUploadAt4(param_1 + param_2 * 400 + 0x3a80);
      vifObjectMatrix(auStack_70);
      vifCallModel((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177f478 + uVar6 * 0x18))) + 0x10);
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177f480 + uVar6 * 0x18)));
    }
    else {
      if ((long)(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5404))) != uVar2) {
        return 0;
      }
      vifDirect(param_1 + 0x3300,2);
      vifUploadAt16(0x177f5e0);
      UploadPackedColors((*(uint32_t*)rtguest::G((uint32_t)0x177ea50 + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5410)))) * 4)));
      vifUploadAt4(param_1 + param_2 * 400 + 0x3a80);
      vifObjectMatrix(auStack_70);
      vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x177f310 + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5410))) * 6) * 4)) + 0x10);
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177f318 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5410))) * 0x18)));
    }
    break;
  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
  case 10:
  default:
    vifDirect(param_1 + (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + param_1 + 0x5420))) * 0xf0 + 0x2b80,2);
    vifDirect(param_1 + param_2 * 0xf0 + 0x2ba0,0xd);
  case 7:
    vifUploadAt16(0x177f5e0);
    UploadPaintBgColors(
                         ((int)0x177f760 +
                         ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5410))) % 3) * 0x1c +
                         (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x540c))) * 0x3448 + 5));
    vifUploadAt4(param_1 + param_2 * 400 + 0x3a80);
    vifObjectMatrix(auStack_70);
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ab34 + (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + param_1 + 0x5420))) * 4)));
    break;
  case 8:
  case 0xb:
    vifUploadAt16(0x177f5e0);
    vifUploadLightMatrix(0x2a8bb0);
    vifUploadAt4(param_1 + param_2 * 400 + 0x3a80);
    vifObjectMatrix(auStack_70);
    vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x177f310 + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5410))) * 6) * 4)) + 0x10);
    vifCallModel((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177f318 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5410))) * 0x18))) + 0x10);
    psVar5 = (uint32_t)(param_1 + iVar7 + 0x5420);
    if ((*(int16_t*)rtguest::G((uint32_t)psVar5)) == 0) {
    }
    vifUploadAt16(0x177f5e0);
    vifDirect(param_1 + (*(int16_t*)rtguest::G((uint32_t)psVar5)) * 0xf0 + 0x2b80,2);
    UploadPackedColors((*(uint32_t*)rtguest::G((uint32_t)0x177ea50 + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5410)))) * 4)));
    uVar1 = GetPartValueU16((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5400))),(*(int16_t*)rtguest::G((uint32_t)psVar5)));
    SelectCarAttachmentPoint
              (auStack_40,auStack_70,uVar1,
               0x2ed720u +
               (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G((uint32_t)0x177ea50 + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5410)))) * 4)) + 4))) * 0x11);
    vifObjectMatrix(auStack_70);
    iVar3 = GetPartValueU16((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5400))),(*(int16_t*)rtguest::G((uint32_t)psVar5)));
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177eac8 + iVar3 * 4)));
    break;
  case 9:
    vifUploadAt16(0x177f5e0);
    vifUploadLightMatrix(0x2a8bb0);
    vifUploadAt4(param_1 + param_2 * 400 + 0x3a80);
    vifObjectMatrix(auStack_70);
    vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x177f310 + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5410))) * 6) * 4)) + 0x10);
    if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + param_1 + 0x5420))) == 0) {
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177f318 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5410))) * 0x18)));
    }
    else {
      vifUploadAt16(0x177f5e0);
      UploadPackedColors((*(uint32_t*)rtguest::G((uint32_t)0x177ea50 + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5410)))) * 4)));
      vifObjectMatrix(auStack_70);
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177f31c + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5410))) * 0x18)));
    }
    break;
  case 0xc:
    vifUploadAt16(0x177f5e0);
    vifUploadLightMatrix(0x2a8bb0);
    vifUploadAt4(param_1 + param_2 * 400 + 0x3a80);
    vifObjectMatrix(auStack_70);
    vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x177f310 + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5410))) * 6) * 4)) + 0x10);
    vifCallModel((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177f318 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5410))) * 0x18))) + 0x10);
    psVar5 = (uint32_t)(param_1 + iVar7 + 0x5420);
    if ((*(int16_t*)rtguest::G((uint32_t)psVar5)) == 0) {
    }
    vifUploadAt16(0x177f5e0);
    vifObjectMatrix(auStack_70);
    vifDirect(param_1 + (*(int16_t*)rtguest::G((uint32_t)psVar5)) * 0xf0 + 0x2b80,2);
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177f324 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5410))) * 0x18)));
  }
  vifCallModel(iVar3 + 0x10);
}


RT_GUEST_FN(0x00244f60, guest::drawCarouselCar)

}  // namespace guest
