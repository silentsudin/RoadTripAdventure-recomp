// guest:0x00248a70 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateCarouselPromptDialog(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
  char cVar1;
  bool bVar2;
  int iVar3;
  int iVar4;
  
  iVar4 = (int)param_1;
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25))) == '\0') {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 8))) < 0x10) {
      RT_CALLRA(0x248aa0, guest::uiSetTexa((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 8))) << 3));
      bVar2 = false;
    }
    else {
      bVar2 = false;
    }
  }
  else {
    bVar2 = false;
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25))) == '\x02') {
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 8))) < 0x10) {
        RT_CALLRA(0x248ad0, guest::uiSetTexa((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 8))) * -8 + 0x80));
        bVar2 = false;
      }
      else {
        RT_CALLRA(0x248ae0, guest::gs_tasks_kill());
        bVar2 = true;
      }
    }
  }
  if (!bVar2) {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 8))) == 0) {
      RT_CALLRA(0x248b10, guest::gs_tasks_spawn(param_1,0x239150,0x327740));
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x24)));
    }
    else {
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x24)));
    }
    if (cVar1 == '\0') {
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25)));
    }
    else {
      RT_CALLRA(0x248b24, guest::PlayUiSound(0x1a));
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))) == 0) {
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x18)));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = 2;
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25))) = 2;
      }
      else {
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x18)));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = 0xffffffff;
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25))) = 2;
      }
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 8))) = 0;
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25)));
    }
    if (cVar1 == '\0') {
      if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30))) & 0x10) == 0) {
        if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x32))) & 0x10) == 0) {
          return 0;
        }
        if (((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10))) & 2) == 0) {
          return 0;
        }
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x18)));
      }
      else {
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x18)));
      }
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = 0xffffffff;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x25))) = 2;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 8))) = 0;
      RT_CALLRA(0x248bd0, guest::PlayUiSound(0x1b));
    }
  }
  return 0;
}


RT_GUEST_FN(0x00248a70, guest::UpdateCarouselPromptDialog)

}  // namespace guest
