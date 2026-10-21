// guest:0x002092d8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gs_options_options_task(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
  char cVar1;
  byte bVar2;
  int iVar3;
  long lVar4;
  undefined8 uVar5;
  int iVar6;
  
  iVar6 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) == 0) {
    iVar3 = RT_CALLRA(0x209300, guest::uiMenuCreate(param_1) /* $a0 = entry $a0 at the jal (MIPS: no $4 write/jal before 0x209300); pseudocode dropped the live arg */);
    RT_CALLRA(0x20932c, guest::uiMenuAddItem(iVar3,(*(uint32_t*)rtguest::G((uint32_t)(0x29ac90u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 5) * 4))),0x10));
    RT_CALLRA(0x209350, guest::uiMenuAddItem(iVar3,(*(uint32_t*)rtguest::G((uint32_t)(0x29ac94u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 5) * 4))),0x10));
    RT_CALLRA(0x209374, guest::uiMenuAddItem(iVar3,(*(uint32_t*)rtguest::G((uint32_t)(0x29ac98u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 5) * 4))),0x10));
    RT_CALLRA(0x209398, guest::uiMenuAddItem(iVar3,(*(uint32_t*)rtguest::G((uint32_t)(0x29aca0u + (uint32_t)((*(uint32_t*)rtguest::G(0x333284)) * 5) * 4))),0x10));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xe))) = 1;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xc))) = 2;
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = 0xb4;
  }
  else {
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)));
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) != '\0') {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xe))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xe))) & 0x7f;
    }
    lVar4 = (int32_t)RT_CALLRA(0x2093d4, guest::uiMenuRun(iVar3,param_2));
    if (lVar4 != 0) {
      if (lVar4 == -1) {
        (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xd))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xb))) + -1;
      }
      if (lVar4 == -2) {
        RT_CALLRA(0x209408, guest::gs_tasks_kill(param_1));
        bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xe)));
        goto LAB_002094d4;
      }
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xd)));
      if (cVar1 == '\x01') {
        RT_CALLRA(0x209458, guest::PlayUiSound(0x1a));
        uVar5 = 0x208f50;
      }
      else if (cVar1 < '\x02') {
        if (cVar1 != '\0') {
          bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xe)));
          goto LAB_002094d4;
        }
        RT_CALLRA(0x209470, guest::PlayUiSound(0x1a));
        uVar5 = 0x208c98;
      }
      else {
        if (cVar1 != '\x02') {
          if (cVar1 != '\x03') {
            bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xe)));
            goto LAB_002094d4;
          }
          RT_CALLRA(0x209488, guest::PlayUiSound(0x1b));
          iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18)));
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) = 1;
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) = 1;
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xe))) = 2;
          goto LAB_002094d0;
        }
        RT_CALLRA(0x2094b8, guest::PlayUiSound(0x1a));
        uVar5 = 0x2090c0;
      }
      RT_CALLRA(0x20949c, guest::gs_tasks_spawn(param_1,uVar5,0));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xe))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xe))) | 0x80;
    }
  }
LAB_002094d0:
  bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xe)));
LAB_002094d4:
  if ((bVar2 & 0x80) != 0) {
    uiDrawHintBar(3);
    return 0;
  }
  uiDrawHintBar(2);
  return 0;
}


RT_GUEST_FN(0x002092d8, *static_cast<uint32_t (*)(uint32_t, uint32_t, ...)>(&guest::gs_options_options_task))

}  // namespace guest
