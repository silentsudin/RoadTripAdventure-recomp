// guest:0x00211170 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateQuickRaceMenuTask(uint32_t param_1, uint32_t param_2, ...) {
  char cVar1;
  int iVar2;
  
  iVar2 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 8))) == 0) {
    InitProgressRamDefaults();
    gs_tasks_spawn(param_1,0x20dc50,0xa0000);
    (*(uint32_t*)rtguest::G(0x177ea50)) = 0x177f760;
    gs_tasks_spawn(param_1,0x20dc50,0x70001);
    gs_tasks_spawn(param_1,0x20dc50,4);
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24)));
  }
  else if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x25))) == '\0') {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xf))) == '\0') {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x25))) = 5;
      gs_tasks_spawn(param_1,0x2455c0,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x26))));
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24)));
    }
    else {
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24)));
    }
  }
  else {
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24)));
  }
  if (cVar1 != '\0') {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x25))) == '\x05') {
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) == -1) {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 8))) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xc))) = (*(uint8_t*)rtguest::G(0x211948));
      }
      else {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x26))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4)));
        DestroyTaskRecord(param_1);
        (*(uint8_t*)rtguest::G((uint32_t)0x177f760 + 4)) = (*(uint32_t*)rtguest::G((uint32_t)0x29c028 + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x26)))) * 4));
        (*(uint32_t*)rtguest::G(0x177ea50)) = 0x177f760;
        (*(uint32_t*)rtguest::G((uint32_t)0x177f760 + 0)) =
             (*(uint32_t*)rtguest::G((uint32_t)0x17804d0 + (uint32_t)((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x0029c028u + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x26)))) * 4))) * 4));
        gs_tasks_spawn(param_1,0x20dc50,0x70001);
        gs_tasks_spawn(param_1,0x20f950,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x26))));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x25))) = 6;
      }
    }
    else if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x25))) == '\x06') {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x25))) = 0;
      gs_tasks_spawn(param_1,0x20dc50,4);
    }
  }
  return 0;
}


RT_GUEST_FN(0x00211170, guest::UpdateQuickRaceMenuTask)

}  // namespace guest
