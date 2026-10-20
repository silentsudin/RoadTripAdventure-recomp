// guest:0x00208848 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RunMenuPanelTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(112);
  char cVar1;
  short sVar2;
  int iVar3;
uint32_t auStack_70 = rtguest::allocFrame(64);
  
  iVar3 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 0x400000;
    gs_tasks_spawn(param_1,0x20dc50,0x320000);
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25)));
  }
  else {
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25)));
    if ((cVar1 == '\0') && ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xf))) == '\0')) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25))) = 1;
      gs_tasks_spawn(param_1,0x20dc50,0x310000);
      (*(int16_t*)rtguest::G(0x29abe6)) = 0x1100;
      (*(uint32_t*)rtguest::G(0x29abe4)) = 0x1c00;
      PlayUiSound(0x328);
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25)));
    }
  }
  if (cVar1 == '\x01') {
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 2))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 2))) + 1;
    UpdateSceneLightsAndFog(1,0);
    vu0UnitMatrix(auStack_70);
    sVar2 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 2)));
    if (sVar2 < 0x41) {
      (*(float*)rtguest::G(0x29abe0)) = (float)(0x40 - sVar2) * (*(float*)rtguest::G(0x332b2c)) + (*(float*)rtguest::G(0x332b30));
    }
    else if (sVar2 < 0x81) {
      (*(uint32_t*)rtguest::G(0x29abe4)) = (0x80 - sVar2) * 0x70;
      (*(int16_t*)rtguest::G(0x29abe6)) = (0x80 - (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 2)))) * 0x44;
    }
    SetViewDistanceGlobals(0x45000000,0x45000000);
    buildCamera(0x177e6d0,0x29abd0,auStack_70,auStack_70);
    updateLightDirection(0x177e850);
    vifUploadAt4(0x177e6d0);
    vifDirect(0x29ac10,3);
    vifUploadAt16(0x177f5e0);
    vifUploadColours(0x29abf0,0x29ac00);
    vifObjectMatrix(auStack_70);
    vifCallModel((*(uint32_t*)rtguest::G(0x177f310)) + 0x10);
    sVar2 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 2)));
  }
  else {
    sVar2 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 2)));
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x50))) = 0x80fcfcfc;
  if ((0xa4 < sVar2) && ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xf))) == '\0')) {
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x18)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 0x400000;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = 0;
    gs_tasks_kill(param_1);
  }
  return 0;
}


RT_GUEST_FN(0x00208848, guest::RunMenuPanelTask)

}  // namespace guest
