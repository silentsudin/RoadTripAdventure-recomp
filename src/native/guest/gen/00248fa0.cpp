// guest:0x00248fa0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateTaskTimerLimit(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  bool bVar1;
  int iVar2;
  
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) == '\0') {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) < 0x10) {
      uiSetTexa((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) << 3);
      bVar1 = false;
    }
    else {
      bVar1 = false;
    }
  }
  else {
    bVar1 = false;
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) == '\x02') {
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) < 0x10) {
        uiSetTexa((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) * -8 + 0x80);
        bVar1 = false;
      }
      else {
        gs_tasks_kill();
        bVar1 = true;
      }
    }
  }
  if ((!bVar1) && ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) == '\0')) {
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30))) & 0x10) == 0) {
      if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x32))) & 0x10) == 0) {
        return 0;
      }
      if (((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10))) & 2) == 0) {
        return 0;
      }
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x18)));
    }
    else {
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x18)));
    }
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = 0xffffffff;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) = 2;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) = 0;
    PlayUiSound(0x1b);
  }
  return 0;
}


RT_GUEST_FN(0x00248fa0, guest::UpdateTaskTimerLimit)

}  // namespace guest
