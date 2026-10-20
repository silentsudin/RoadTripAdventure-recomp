// guest:0x0026d2c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t StartDayCycleStepTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  undefined4 uVar1;
  int iVar2;
  
  iVar2 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 8))) == 0) {
    gs_music_play_bgm(0xa1a0201);
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x27))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x50))) = 0x80000000;
    uVar1 = gs_tasks_spawn(param_1,0x26ca48,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x27))));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x28))) = uVar1;
  }
  else if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xf))) == '\0') {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 8))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xc))) = 0x26d1c0;
    gs_tasks_spawn(param_1,0x26a928,0);
    PlayBgmSlot1();
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x0026d2c0, guest::StartDayCycleStepTask)

}  // namespace guest
