// guest:0x00248398 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateCarouselConfirmDialog(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
  int iVar1;
  bool bVar2;
  uint64_t uVar3;
  uint64_t lVar4;
  char cVar5;
  int iVar6;
  int iVar7;
  
  iVar7 = (int)param_1;
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25))) == '\0') {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8))) < 0x10) {
      RT_CALLRA(0x2483d0, guest::uiSetTexa((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8))) << 3));
      bVar2 = false;
    }
    else {
      bVar2 = false;
    }
  }
  else {
    bVar2 = false;
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25))) == '\x02') {
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8))) < 0x10) {
        RT_CALLRA(0x248400, guest::uiSetTexa((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8))) * -8 + 0x80));
        bVar2 = false;
      }
      else {
        RT_CALLRA(0x248410, guest::gs_tasks_kill());
        bVar2 = true;
      }
    }
  }
  if (!bVar2) {
    RT_CALLRA(0x248424, guest::DrawDualScoreNumbers());
    iVar6 = (int)param_2;
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8))) == 0) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x23))) = 9;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x21))) = 5;
      uVar3 = RT_CALLRA(0x248448, guest::uiMenuCreate(param_1));
      RT_CALLRA(0x248460, guest::uiMenuAddItem(uVar3,0x327710,0x10));
      RT_CALLRA(0x248474, guest::uiMenuAddItem(uVar3,0x327728,0x10));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar3 + 0xc))) = 2;
      cVar5 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25)));
    }
    else {
      cVar5 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25)));
      if (cVar5 == '\0') {
        iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28)));
        lVar4 = RT_CALLRA(0x24849c, guest::uiMenuRun(iVar1,param_2));
        if (lVar4 == 1) {
          RT_CALLRA(0x2484c0, guest::gs_tasks_spawn(param_1,0x269e50,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0xd)))));
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25))) = 1;
          RT_CALLRA(0x2484cc, guest::PlayUiSound(0x1a));
          cVar5 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25)));
        }
        else {
          cVar5 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25)));
        }
      }
      else if ((cVar5 == '\x01') && ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x24))) != '\0')) {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25))) = 0;
        cVar5 = '\0';
      }
    }
    if (cVar5 == '\0') {
      if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x30))) & 0x10) == 0) {
        if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x32))) & 0x10) == 0) {
          return 0;
        }
        if (((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 10))) & 2) == 0) {
          return 0;
        }
        iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x18)));
      }
      else {
        iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x18)));
      }
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) = 0xffffffff;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x25))) = 2;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8))) = 0;
      RT_CALLRA(0x24855c, guest::PlayUiSound(0x1b));
    }
  }
  return 0;
}


RT_GUEST_FN(0x00248398, guest::UpdateCarouselConfirmDialog)

}  // namespace guest
