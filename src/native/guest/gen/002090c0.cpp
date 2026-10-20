// guest:0x002090c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gs_options_sub_tasks_volume(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
  }
  int iVar1;
  char cVar2;
  undefined8 uVar3;
  long lVar4;
uint32_t pbVar5;
  int iVar6;
  int iVar7;
  
  iVar7 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8))) == 0) {
    uVar3 = RT_CALLRA(0x2090e4, guest::uiMenuCreate());
    RT_CALLRA(0x2090fc, guest::uiMenuAddItem(uVar3,0x3332e0,0x10));
    RT_CALLRA(0x209110, guest::uiMenuAddItem(uVar3,0x3332e8,0x10));
    RT_CALLRA(0x209124, guest::uiMenuAddItem(uVar3,0x3332f0,0x10));
    RT_CALLRA(0x209138, guest::uiMenuAddItem(uVar3,0x3332f8,0x10));
    RT_CALLRA(0x20914c, guest::uiMenuAddItem(uVar3,0x333300,0x10));
    RT_CALLRA(0x209160, guest::uiMenuAddItem(uVar3,0x333308,0x10));
    RT_CALLRA(0x20916c, guest::CenterRectangle(uVar3,8));
    iVar6 = (int)uVar3;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xe))) = 1;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xc))) = 2;
    cVar2 = (char)(*(uint32_t*)rtguest::G(0x333290));
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xd))) = (char)(*(uint32_t*)rtguest::G(0x333290));
    pbVar5 = (uint32_t)(iVar6 + cVar2 + 0x50);
    (*(int8_t*)rtguest::G((uint32_t)pbVar5)) = (*(int8_t*)rtguest::G((uint32_t)pbVar5)) | 2;
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8))) == 0) {
      return 0;
    }
  }
  iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28)));
  lVar4 = RT_CALLRA(0x2091bc, guest::uiMenuRun(iVar6,param_2));
  if (lVar4 != 0) {
    if (lVar4 == -2) {
      gs_tasks_kill(param_1);
      return 0;
    }
    if (lVar4 == -1) {
      RT_CALLRA(0x209290, guest::PlayUiSound(0x1b));
    }
    else {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x50))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x50))) & 0xfd;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x51))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x51))) & 0xfd;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x52))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x52))) & 0xfd;
      pbVar5 = (uint32_t)(iVar6 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xd))) + 0x50);
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x53))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x53))) & 0xfd;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x55))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x55))) & 0xfd;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x54))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x54))) & 0xfd;
      (*(int8_t*)rtguest::G((uint32_t)pbVar5)) = (*(int8_t*)rtguest::G((uint32_t)pbVar5)) | 2;
      (*(uint32_t*)rtguest::G(0x333290)) = (int)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xd)));
      iVar1 = ((5 - (*(uint32_t*)rtguest::G(0x333290))) * 0x3fff) / 5;
      RT_CALLRA(0x209288, guest::SetVolumeDgaRegs(iVar1,iVar1));
      RT_CALLRA(0x2092a0, guest::PlayUiSound(0x2f));
    }
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xe))) = 2;
    iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 4))) = 0;
  }
  return 0;
}


RT_GUEST_FN(0x002090c0, guest::gs_options_sub_tasks_volume)

}  // namespace guest
