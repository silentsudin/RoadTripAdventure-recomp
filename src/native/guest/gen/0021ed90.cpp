// guest:0x0021ed90 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CameraWaitOrCutsceneState(uint32_t param_1, uint32_t param_2, ...) {
  char cVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
uint32_t auStack_60 = rtguest::allocFrame(48);
uint32_t auStack_30 = rtguest::allocFrame(16);
  
  iVar3 = (int)param_1;
  uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x1c)));
  iVar4 = (int)param_2;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) == 0) {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x27))) = 0xff;
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x22)));
  }
  else {
    if (-1 < (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x28)))) {
      InitializeTask(param_1,0x21e690);
      RunCarCameraTask(param_1,param_2);
      return 0;
    }
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x22)));
  }
  if (cVar1 == '\x17') {
    vu0UnitMatrix(auStack_60);
    vu0CopyVector(auStack_30,0x177eb40);
    buildCamera(0x177e6d0 + (uVar2 & 1) * 400,0x29cc80,auStack_60,auStack_60);
  }
  else if (cVar1 == '\x18') {
    buildCamera(0x177e6d0 + (uVar2 & 1) * 400,0x29cca0,0,0);
  }
  return 0;
}


RT_GUEST_FN(0x0021ed90, guest::CameraWaitOrCutsceneState)

}  // namespace guest
