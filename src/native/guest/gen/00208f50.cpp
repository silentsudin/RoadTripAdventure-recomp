// guest:0x00208f50 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gs_options_sub_tasks_speaker(uint32_t param_1, uint32_t param_2, ...) {
  char cVar1;
  undefined8 uVar2;
  long lVar3;
uint32_t pbVar4;
  int iVar5;
  int iVar6;
  
  iVar6 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) == 0) {
    uVar2 = uiMenuCreate();
    uiMenuAddItem(uVar2,0x3332d0,0x10);
    uiMenuAddItem(uVar2,0x3332d8,0x10);
    CenterRectangle(uVar2,8);
    iVar5 = (int)uVar2;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xe))) = 1;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xc))) = 2;
    cVar1 = SoundRpcGetStatus();
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xd))) = cVar1;
    pbVar4 = (uint32_t)(iVar5 + cVar1 + 0x50);
    (*(int8_t*)rtguest::G((uint32_t)pbVar4)) = (*(int8_t*)rtguest::G((uint32_t)pbVar4)) | 2;
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) == 0) {
      return 0;
    }
  }
  iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)));
  lVar3 = uiMenuRun(iVar5,param_2);
  if (lVar3 != 0) {
    if (lVar3 == -2) {
      gs_tasks_kill(param_1);
    }
    if (lVar3 == -1) {
      PlayUiSound(0x1b);
    }
    else {
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x50))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x50))) & 0xfd;
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x51))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x51))) & 0xfd;
      gs_options_speaker_set((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xd))));
      PlayUiSound(0x1f);
      pbVar4 = (uint32_t)(iVar5 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xd))) + 0x50);
      (*(int8_t*)rtguest::G((uint32_t)pbVar4)) = (*(int8_t*)rtguest::G((uint32_t)pbVar4)) | 2;
    }
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xe))) = 2;
    iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) = 0;
  }
}


RT_GUEST_FN(0x00208f50, guest::gs_options_sub_tasks_speaker)

}  // namespace guest
