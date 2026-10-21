// guest:0x0023a320 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t HandleNumberSpinner(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
  }
  byte bVar1;
uint32_t pcVar2;
  ushort uVar3;
  undefined1 uVar4;
  int iVar5;
  
  iVar5 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) == 0) {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26))) = 1;
  }
  else {
    RT_CALLRA(0x23a360, guest::uiDrawPanel9(0x40,0x20,0x3c,0xc,2));
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) & 0x10) != 0) {
      RT_CALLRA(0x23a37c, guest::uiDrawIcon20(0x58,0x28,2));
      RT_CALLRA(0x23a38c, guest::uiDrawIcon20(0x58,0x58,3));
    }
    uVar3 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x34)));
    if (((uVar3 & 0x1000) != 0) && ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26))) != '\0')) {
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26))) + -1;
      uVar3 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x34)));
    }
    if ((uVar3 & 0x4000) == 0) {
      uVar4 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26)));
    }
    else if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26))) < 99) {
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26))) + 1;
      uVar4 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26)));
    }
    else {
      uVar4 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26)));
    }
    RT_CALLRA(0x23a3e8, guest::uiDrawNumber16(0x70,0x20,uVar4,10));
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30))) & 0x40) != 0) {
      pcVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c)));
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26))) == (*(int8_t*)rtguest::G((uint32_t)pcVar2))) {
        bVar1 = (*(int8_t*)rtguest::G((uint32_t)pcVar2 + (uint32_t)(1) * sizeof(int8_t)));
      }
      else {
        bVar1 = (*(int8_t*)rtguest::G((uint32_t)pcVar2 + (uint32_t)(2) * sizeof(int8_t)));
      }
      iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) = (uint32_t)bVar1;
      RT_CALLRA(0x23a428, guest::PlayUiSound(0x1a));
      gs_tasks_kill(param_1);
      return 0;
    }
  }
  return 0;
}


RT_GUEST_FN(0x0023a320, guest::HandleNumberSpinner)

}  // namespace guest
