// guest:0x00205088 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateMenuFlowTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(1, (uint32_t)param_2);  // walk: = param_2
  }
  byte bVar1;
  int iVar2;
  int iVar3;
  
  RT_CALLRA(0x2050a0, guest::UpdateMenuInputState(param_2));
  RT_CALLRA(0x2050a8, guest::UpdateControllerInput(param_2));
  RT_CALLRA(0x2050b0, guest::ClearInputTransientState(param_2));
  iVar3 = (int)param_1;
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) != '\0') {
    bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25)));
    if (bVar1 == 1) {
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25))) = 2;
      if (iVar2 == 1) {
        gs_tasks_spawn(param_1,0x26d370,0);
        return 0;
      }
      if (iVar2 < 2) {
        if (iVar2 == 0) {
          gs_tasks_spawn(param_1,0x26d438,0);
          return 0;
        }
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25))) = 0;
      }
      else {
        if (iVar2 == 2) {
          gs_tasks_spawn(param_1,0x26b848,0);
          return 0;
        }
        if (iVar2 == 3) {
          gs_tasks_spawn(param_1,0x211320,0);
          return 0;
        }
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25))) = 0;
      }
      gs_tasks_spawn(param_1,0x20a2b0,2);
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x28))) = 0;
    }
    else if (bVar1 < 2) {
      if (bVar1 == 0) {
        iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4)));
        if (iVar2 == 1) {
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25))) = 2;
          gs_tasks_spawn(param_1,0x211170,0);
          return 0;
        }
        if (iVar2 < 2) {
          if (iVar2 == 0) {
            (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25))) = 1;
            gs_tasks_spawn(param_1,0x209520,0);
            return 0;
          }
        }
        else if (iVar2 == 2) {
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25))) = 1;
          gs_tasks_spawn(param_1,0x209848,0);
          return 0;
        }
      }
    }
    else if (bVar1 == 2) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25))) = 0;
      gs_tasks_spawn(param_1,0x20a2b0,3);
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x28))) = 0;
      RT_CALLRA(0x2052b4, guest::SoundInitVolumes());
    }
  }
  return 0;
}


RT_GUEST_FN(0x00205088, guest::UpdateMenuFlowTask)

}  // namespace guest
