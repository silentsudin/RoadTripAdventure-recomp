// guest:0x00244f60 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawCarouselCar(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(112);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x68) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x6c) = 0x00000000u;
  }
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
  RT_CALLRA(0x244f94, guest::vifDirect(param_1 + 0x60,2));
  RT_CALLRA(0x244fb4, guest::vifDirect(param_1 + param_2 * 0xd0 + 0x80,0xd));
  uVar1 = RT_CALLRA(0x244fc0, guest::DispatchByIndex16((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5400))),(*(int16_t*)rtguest::G((uint32_t)psVar5))));
  iVar3 = param_2 * 8 + param_1;
  uVar4 = 5;
  if ((long)(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5404))) != (long)(*(int16_t*)rtguest::G((uint32_t)psVar5))) {
    uVar4 = 0;
  }
  RT_CALLRA(0x245000, guest::uiDrawTextScaled((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x5380))),(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x5382))),
                   (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x5384))),uVar1,uVar4,(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x5386)))));
  RT_CALLRA(0x245008, guest::vu0UnitMatrix(auStack_70));
  switch((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5400)))) {
  case 0:
    uVar6 = param_2 & 7;
    psVar5 = (uint32_t)(param_1 + iVar7 + 0x5420);
    uVar2 = (uint32_t)(*(int16_t*)rtguest::G((uint32_t)psVar5));
    if ((uint32_t)(*(uint8_t*)rtguest::G((uint32_t)0x335720 + (uint32_t)(uVar6) * 1)) == uVar2) {
      RT_CALLRA(0x245070, guest::vifDirect(param_1 + uVar6 * 0xf0 + 0x2b80,2));
      RT_CALLRA(0x24507c, guest::vifUploadAt16(0x177f5e0));
      RT_CALLRA(0x2450b0, guest::UploadPackedColors(0x017804d0 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x540c))) * 0xd12 + (int)(*(int16_t*)rtguest::G((uint32_t)psVar5))));
      RT_CALLRA(0x2450d4, guest::vifUploadAt4(param_1 + param_2 * 400 + 0x3a80));
      RT_CALLRA(0x2450dc, guest::vifObjectMatrix(auStack_70));
      RT_CALLRA(0x2450f0, guest::vifCallModel((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177f478 + uVar6 * 0x18))) + 0x10));
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177f480 + uVar6 * 0x18)));
    }
    else {
      if ((long)(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5404))) != uVar2) {
        return 0;
      }
      RT_CALLRA(0x24510c, guest::vifDirect(param_1 + 0x3300,2));
      RT_CALLRA(0x24511c, guest::vifUploadAt16(0x177f5e0));
      RT_CALLRA(0x245138, guest::UploadPackedColors((*(uint32_t*)rtguest::G((uint32_t)0x177ea50 + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5410)))) * 4))));
      RT_CALLRA(0x245154, guest::vifUploadAt4(param_1 + param_2 * 400 + 0x3a80));
      RT_CALLRA(0x24515c, guest::vifObjectMatrix(auStack_70));
      RT_CALLRA(0x24517c, guest::vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x177f310 + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5410))) * 6) * 4)) + 0x10));
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
    RT_CALLRA(0x2452fc, guest::vifDirect(param_1 + (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + param_1 + 0x5420))) * 0xf0 + 0x2b80,2));
    RT_CALLRA(0x24552c, guest::vifDirect(param_1 + param_2 * 0xf0 + 0x2ba0,0xd));
    return 0;
  case 7:
    RT_CALLRA(0x2451a4, guest::vifUploadAt16(0x177f5e0));
    RT_CALLRA(0x2451ec, guest::UploadPaintBgColors((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x177f760 +
                         ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5410))) % 3) * 0x1c +
                         (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x540c))) * 0x3448 + 5)))));
    RT_CALLRA(0x245208, guest::vifUploadAt4(param_1 + param_2 * 400 + 0x3a80));
    RT_CALLRA(0x245210, guest::vifObjectMatrix(auStack_70));
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ab34 + (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + param_1 + 0x5420))) * 4)));
    break;
  case 8:
  case 0xb:
    RT_CALLRA(0x245248, guest::vifUploadAt16(0x177f5e0));
    RT_CALLRA(0x245254, guest::vifUploadLightMatrix(0x2a8bb0));
    RT_CALLRA(0x245270, guest::vifUploadAt4(param_1 + param_2 * 400 + 0x3a80));
    RT_CALLRA(0x245278, guest::vifObjectMatrix(auStack_70));
    RT_CALLRA(0x245298, guest::vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x177f310 + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5410))) * 6) * 4)) + 0x10));
    RT_CALLRA(0x2452b8, guest::vifCallModel((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177f318 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5410))) * 0x18))) + 0x10));
    psVar5 = (uint32_t)(param_1 + iVar7 + 0x5420);
    if ((*(int16_t*)rtguest::G((uint32_t)psVar5)) == 0) {
      return 0;
    }
    RT_CALLRA(0x2452d8, guest::vifUploadAt16(0x177f5e0));
    RT_CALLRA(0x24557c, guest::vifDirect(param_1 + (*(int16_t*)rtguest::G((uint32_t)psVar5)) * 0xf0 + 0x2b80,2));
    RT_CALLRA(0x245310, guest::UploadPackedColors((*(uint32_t*)rtguest::G((uint32_t)0x177ea50 + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5410)))) * 4))));
    uVar1 = RT_CALLRA(0x24531c, guest::GetPartValueU16((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5400))),(*(int16_t*)rtguest::G((uint32_t)psVar5))));
    RT_CALLRA(0x245358, guest::SelectCarAttachmentPoint(auStack_40,auStack_70,uVar1,
               0x2ed720u +
               (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G((uint32_t)0x177ea50 + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5410)))) * 4)) + 4))) * 0x11));
    RT_CALLRA(0x245360, guest::vifObjectMatrix(auStack_70));
    iVar3 = RT_CALLRA(0x24536c, guest::GetPartValueU16((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5400))),(*(int16_t*)rtguest::G((uint32_t)psVar5))));
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177eac8 + iVar3 * 4)));
    break;
  case 9:
    RT_CALLRA(0x245398, guest::vifUploadAt16(0x177f5e0));
    RT_CALLRA(0x2453a4, guest::vifUploadLightMatrix(0x2a8bb0));
    RT_CALLRA(0x2453c0, guest::vifUploadAt4(param_1 + param_2 * 400 + 0x3a80));
    RT_CALLRA(0x2453c8, guest::vifObjectMatrix(auStack_70));
    RT_CALLRA(0x2453f0, guest::vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x177f310 + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5410))) * 6) * 4)) + 0x10));
    if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + param_1 + 0x5420))) == 0) {
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177f318 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5410))) * 0x18)));
    }
    else {
      RT_CALLRA(0x245408, guest::vifUploadAt16(0x177f5e0));
      RT_CALLRA(0x245424, guest::UploadPackedColors((*(uint32_t*)rtguest::G((uint32_t)0x177ea50 + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5410)))) * 4))));
      RT_CALLRA(0x24542c, guest::vifObjectMatrix(auStack_70));
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177f31c + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5410))) * 0x18)));
    }
    break;
  case 0xc:
    RT_CALLRA(0x245470, guest::vifUploadAt16(0x177f5e0));
    RT_CALLRA(0x24547c, guest::vifUploadLightMatrix(0x2a8bb0));
    RT_CALLRA(0x245498, guest::vifUploadAt4(param_1 + param_2 * 400 + 0x3a80));
    RT_CALLRA(0x2454a0, guest::vifObjectMatrix(auStack_70));
    RT_CALLRA(0x2454c8, guest::vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x177f310 + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5410))) * 6) * 4)) + 0x10));
    RT_CALLRA(0x2454e8, guest::vifCallModel((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177f318 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5410))) * 0x18))) + 0x10));
    psVar5 = (uint32_t)(param_1 + iVar7 + 0x5420);
    if ((*(int16_t*)rtguest::G((uint32_t)psVar5)) == 0) {
      return 0;
    }
    RT_CALLRA(0x245504, guest::vifUploadAt16(0x177f5e0));
    RT_CALLRA(0x24550c, guest::vifObjectMatrix(auStack_70));
    RT_CALLRA(0x245598, guest::vifDirect(param_1 + (*(int16_t*)rtguest::G((uint32_t)psVar5)) * 0xf0 + 0x2b80,2));
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177f324 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5410))) * 0x18)));
  }
  RT_CALLRA(0x24554c, guest::vifCallModel(iVar3 + 0x10));
  return 0;
}


RT_GUEST_FN(0x00244f60, guest::drawCarouselCar)

}  // namespace guest
