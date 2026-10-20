// guest:0x0022ca28 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RunCourseSelectMenuTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(160);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x90) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x94) = 0x00000000u;
  }
uint32_t puVar1;
  undefined1 uVar2;
  uint uVar3;
  int iVar4;
  undefined4 uVar5;
  long lVar6;
  undefined8 uVar7;
  int iVar8;
  uint uVar9;
  int iVar10;
uint32_t puVar11;
uint32_t puVar12;
uint32_t auStack_a0 = rtguest::allocFrame(96);
  
  puVar12 = auStack_a0;
  iVar8 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 8))) == 0) {
    uVar9 = 0;
    puVar11 = 0x0029f440;
    uVar2 = (*(uint8_t*)rtguest::G(0x29f440));
    while( true ) {
      puVar1 = puVar11 + 1;
      puVar11 = puVar11 + 2;
      lVar6 = RT_CALLRA(0x22ca74, guest::TestStampFlag(uVar2,(*(uint8_t*)rtguest::G((uint32_t)puVar1))));
      uVar3 = uVar9 & 0x1f;
      uVar9 = uVar9 + 1;
      if (lVar6 == 0) {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x20))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x20))) | 1 << uVar3;
        (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))) + '\x01';
      }
      if (0x13 < (int)uVar9) break;
      uVar2 = (*(uint8_t*)rtguest::G((uint32_t)puVar11));
    }
  }
  iVar4 = 0;
  uVar9 = 0;
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))) != '\0') {
    do {
      iVar10 = iVar4 + 1;
      iVar4 = RT_CALLRA(0x22cad8, guest::FindNthSetBit((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x20))),iVar4));
      uVar9 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26)));
      (*(uint32_t*)rtguest::G((uint32_t)puVar12)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x2be398 + (uint32_t)((uint32_t)(*(uint8_t*)rtguest::G((uint32_t)0x2be018 + (uint32_t)(iVar4 * 0x10) * 1))) * 4));
      puVar12 = puVar12 + sizeof(uint32_t);
      iVar4 = iVar10;
    } while (iVar10 < (int)uVar9);
  }
  iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 8)));
  (*(uint32_t*)rtguest::G((uint32_t)auStack_a0 + (uint32_t)(uVar9) * 4)) = 0x003336e8;
  if (iVar4 == 0) {
    uVar7 = RT_CALLRA(0x22cb28, guest::uiMenuCreate(param_1));
    RT_CALLRA(0x22cb44, guest::LayoutTextItems(uVar7,auStack_a0,(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))) + 1,10));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar7 + 0x5f))) = 2;
    RT_CALLRA(0x22cb58, guest::CenterRectangle(uVar7,5));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar7 + 0xe))) = 1;
  }
  else {
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
    lVar6 = RT_CALLRA(0x22cb7c, guest::uiMenuWindow(iVar4,param_2,auStack_a0));
    if (lVar6 != 0) {
      if (lVar6 == -2) {
        RT_CALLRA(0x22cbd0, guest::gs_tasks_kill(param_1));
      }
      else if ((lVar6 == -1) || ((long)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xd))) == (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))))) {
        RT_CALLRA(0x22cbc8, guest::PlayUiSound(0x1b));
        iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x18)));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x24))) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 4))) = 0xffffffff;
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xe))) = 2;
      }
      else {
        uVar5 = RT_CALLRA(0x22cbb0, guest::FindNthSetBit((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x20)))));
        iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x18)));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x24))) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 4))) = uVar5;
        RT_CALLRA(0x22cbe0, guest::PlayUiSound(0x1a));
        RT_CALLRA(0x22cc08, guest::gs_tasks_kill(param_1));
      }
    }
  }
  return 0;
}


RT_GUEST_FN(0x0022ca28, guest::RunCourseSelectMenuTask)

}  // namespace guest
