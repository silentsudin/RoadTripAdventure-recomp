// guest:0x0021ed90 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CameraWaitOrCutsceneState(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x48, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x50, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x58) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x5c) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(1, (uint32_t)param_2);  // walk: = param_2
  }
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
      RT_CALLRA(0x21edfc, guest::InitializeTask(param_1,0x21e690));
      RT_CALLRA(0x21ee08, guest::RunCarCameraTask(param_1,param_2));
      return 0;
    }
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x22)));
  }
  if (cVar1 == '\x17') {
    RT_CALLRA(0x21ee28, guest::vu0UnitMatrix(auStack_60));
    RT_CALLRA(0x21ee38, guest::vu0CopyVector(auStack_30,0x177eb40));
    RT_CALLRA(0x21ee50, guest::buildCamera(0x177e6d0 + (uVar2 & 1) * 400,0x29cc80,auStack_60,auStack_60));
  }
  else if (cVar1 == '\x18') {
    RT_CALLRA(0x21ee78, guest::buildCamera(0x177e6d0 + (uVar2 & 1) * 400,0x29cca0,0,0));
  }
  return 0;
}


RT_GUEST_FN(0x0021ed90, guest::CameraWaitOrCutsceneState)

}  // namespace guest
