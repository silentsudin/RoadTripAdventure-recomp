// guest:0x00211320 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateTwoPlayerMenuTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
  }
  char cVar1;
  int iVar2;
  
  iVar2 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 8))) == 0) {
    RT_CALLRA(0x211340, guest::LookupPlayerNameSlot(param_2));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x25))) = 7;
  }
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x25))) == '\x04') {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0xf))) == '\0') {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x25))) = 5;
      RT_CALLRA(0x21137c, guest::gs_tasks_spawn(param_1,0x2455c0,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x26)))));
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24)));
    }
    else {
      RT_CALLRA(0x211390, guest::uiFade(0x8000000));
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
        RT_CALLRA(0x2113d4, guest::DestroyTaskRecord(param_1));
        RT_CALLRA(0x2113e8, guest::gs_tasks_spawn(param_1,0x20f950,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x26)))));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x25))) = 6;
      }
    }
    else {
      if (cVar1 != '\x06') goto LAB_0021143c;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x25))) = 4;
      RT_CALLRA(0x21142c, guest::gs_tasks_spawn(param_1,0x20dc50,5));
    }
  }
  cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x25)));
LAB_0021143c:
  if (cVar1 == '\a') {
    RT_CALLRA(0x211458, guest::gs_tasks_spawn(param_1,0x20dc50,0xa000a));
    (*(uint32_t*)rtguest::G(0x177ea64)) = 0x01782be0;
    (*(uint32_t*)rtguest::G(0x177ea54)) = 0x177f77c;
    (*(uint32_t*)rtguest::G(0x177ea58)) = 0x0177f798;
    (*(int32_t*)rtguest::G(0x177ea5c)) = 0x01782ba8;
    (*(uint32_t*)rtguest::G(0x177ea60)) = 0x1782bc4;
    (*(uint32_t*)rtguest::G(0x177ea50)) = 0x177f760;
    RT_CALLRA(0x2114a8, guest::gs_tasks_spawn(param_1,0x20dc50,0x70006));
    RT_CALLRA(0x2114b8, guest::gs_tasks_spawn(param_1,0x20dc50,5));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x25))) = 4;
  }
  return 0;
}


RT_GUEST_FN(0x00211320, guest::UpdateTwoPlayerMenuTask)

}  // namespace guest
