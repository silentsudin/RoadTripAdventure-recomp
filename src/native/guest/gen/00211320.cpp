// guest:0x00211320 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateTwoPlayerMenuTask(uint32_t param_1, uint32_t param_2, ...) {
  char cVar1;
  int iVar2;
  
  iVar2 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 8))) == 0) {
    LookupPlayerNameSlot(param_2);
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x25))) = 7;
  }
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x25))) == '\x04') {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0xf))) == '\0') {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x25))) = 5;
      gs_tasks_spawn(param_1,0x2455c0,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x26))));
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24)));
    }
    else {
      uiFade(0x8000000);
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24)));
    }
  }
  else {
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24)));
  }
  if (cVar1 != '\0') {
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x25)));
    if (cVar1 == '\x05') {
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) == -1) {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 8))) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xc))) = 0x211948;
      }
      else {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x26))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4)));
        DestroyTaskRecord(param_1);
        gs_tasks_spawn(param_1,0x20f950,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x26))));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x25))) = 6;
      }
    }
    else {
      if (cVar1 != '\x06') goto LAB_0021143c;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x25))) = 4;
      gs_tasks_spawn(param_1,0x20dc50,5);
    }
  }
  cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x25)));
LAB_0021143c:
  if (cVar1 == '\a') {
    gs_tasks_spawn(param_1,0x20dc50,0xa000a);
    (*(uint32_t*)rtguest::G(0x177ea64)) = 0x01782be0;
    (*(uint32_t*)rtguest::G(0x177ea54)) = 0x177f77c;
    (*(uint32_t*)rtguest::G(0x177ea58)) = 0x0177f798;
    (*(int32_t*)rtguest::G(0x177ea5c)) = 0x01782ba8;
    (*(uint32_t*)rtguest::G(0x177ea60)) = 0x1782bc4;
    (*(uint32_t*)rtguest::G(0x177ea50)) = 0x177f760;
    gs_tasks_spawn(param_1,0x20dc50,0x70006);
    gs_tasks_spawn(param_1,0x20dc50,5);
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x25))) = 4;
  }
  return 0;
}


RT_GUEST_FN(0x00211320, guest::UpdateTwoPlayerMenuTask)

}  // namespace guest
