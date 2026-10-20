// guest:0x0026cb60 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t StartAreaIntroDayCycle(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  short sVar1;
  undefined4 uVar2;
  char cVar3;
  ulong uVar4;
  int iVar5;
  
  iVar5 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) == 0) {
    uVar2 = gs_tasks_spawn(param_1,0x26ca48,0);
    uVar4 = 0xa1c0000;
  }
  else {
    cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25)));
    if (cVar3 == '\0') {
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xf))) == '\0') {
        PlayBgmSlot1();
        (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 2))) = 0;
        cVar3 = '\x01';
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25))) = 1;
        goto LAB_0026cca4;
      }
      sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 2)));
    }
    else {
      sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 2)));
    }
    if (sVar1 == 0x30c) {
      gs_tasks_spawn(param_1,0x2082d8,0);
      PlayBgmSlot5();
      cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25)));
      goto LAB_0026cca4;
    }
    if (sVar1 != 0x34c) {
      if ((cVar3 == '\x02') && ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xf))) == '\0')) {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xc))) = 0x26d1c0;
        gs_tasks_spawn(param_1,0x26a928,0);
        PlayBgmSlot1();
        cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25)));
      }
      goto LAB_0026cca4;
    }
    gs_tasks_kill((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28))));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x27))) = 1;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25))) = 2;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x50))) = 0x80000000;
    uVar2 = gs_tasks_spawn(param_1,0x26ca48,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x27))));
    uVar4 = 0xa1a0000;
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28))) = uVar2;
  gs_music_play_bgm(uVar4 | 0x201);
  cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25)));
LAB_0026cca4:
  if (cVar3 != '\0') {
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 2))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 2))) + 1;
  }
  return 0;
}


RT_GUEST_FN(0x0026cb60, guest::StartAreaIntroDayCycle)

}  // namespace guest
