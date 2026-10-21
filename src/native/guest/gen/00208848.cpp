// guest:0x00208848 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RunMenuPanelTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(112);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x48, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x58, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x50, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x60) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x64) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(1, (uint32_t)param_2);  // walk: = param_2
  }
  char cVar1;
  short sVar2;
  int iVar3;
uint32_t auStack_70 = rtguest::allocFrame(64);
  
  iVar3 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 0x400000;
    RT_CALLRA(0x208890, guest::gs_tasks_spawn(param_1,0x20dc50,0x320000));
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25)));
  }
  else {
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25)));
    if ((cVar1 == '\0') && ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xf))) == '\0')) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25))) = 1;
      RT_CALLRA(0x2088cc, guest::gs_tasks_spawn(param_1,0x20dc50,0x310000));
      (*(int16_t*)rtguest::G(0x29abe6)) = 0x1100;
      // sh 0x1c00,0x14($3) @0x2088ec (MIPS; fn/00208848.c:38 DAT_0029abe4): a HALFWORD store, the u32 store zeroed the 0x29abe6 halfword just set
      (*(int16_t*)rtguest::G(0x29abe4)) = 0x1c00;
      RT_CALLRA(0x2088e8, guest::PlayUiSound(0x328));
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25)));
    }
  }
  if (cVar1 == '\x01') {
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 2))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 2))) + 1;
    RT_CALLRA(0x208910, guest::UpdateSceneLightsAndFog(1,0));
    RT_CALLRA(0x208918, guest::vu0UnitMatrix(auStack_70));
    sVar2 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 2)));
    if (sVar2 < 0x41) {
      (*(float*)rtguest::G(0x29abe0)) = (float)(0x40 - sVar2) * (*(float*)rtguest::G(0x332b2c)) + (*(float*)rtguest::G(0x332b30));
    }
    else if (sVar2 < 0x81) {
      // sh $2,0x14($5) @0x208988 (MIPS; fn/00208848.c:47): halfword store
      (*(int16_t*)rtguest::G(0x29abe4)) = (0x80 - sVar2) * 0x70;
      (*(int16_t*)rtguest::G(0x29abe6)) = (0x80 - (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 2)))) * 0x44;
    }
    RT_CALLRA(0x2089ac, guest::SetViewDistanceGlobals(0x45000000,0x45000000));
    RT_CALLRA(0x2089c0, guest::buildCamera(0x177e6d0,0x29abd0,auStack_70,auStack_70));
    RT_CALLRA(0x2089c8, guest::updateLightDirection(0x177e850));
    RT_CALLRA(0x2089d0, guest::vifUploadAt4(0x177e6d0));
    RT_CALLRA(0x2089e0, guest::vifDirect(0x29ac10,3));
    RT_CALLRA(0x2089ec, guest::vifUploadAt16(0x177f5e0));
    RT_CALLRA(0x208a00, guest::vifUploadColours(0x29abf0,0x29ac00));
    RT_CALLRA(0x208a08, guest::vifObjectMatrix(auStack_70));
    RT_CALLRA(0x208a18, guest::vifCallModel((*(uint32_t*)rtguest::G(0x177f310)) + 0x10));
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
    RT_CALLRA(0x208a64, guest::gs_tasks_kill(param_1));
  }
  return 0;
}


RT_GUEST_FN(0x00208848, guest::RunMenuPanelTask)

}  // namespace guest
