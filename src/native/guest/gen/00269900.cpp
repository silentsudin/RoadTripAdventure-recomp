// guest:0x00269900 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t shopBodySellMenuTask(uint32_t param_1, uint32_t param_2, ...) {
  byte bVar1;
  int iVar2;
  undefined1 uVar3;
  char cVar4;
  int iVar5;
  undefined8 uVar6;
  undefined8 uVar7;
  long lVar8;
  uint uVar9;
  int iVar10;
  uint uVar11;
  int iVar12;
  
  iVar10 = (int)param_1;
  uVar11 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x1c))) & 1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 8))) == 0) {
    uiMenuCreate(param_1);
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x27))) = 0;
    return 0;
  }
  bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x25)));
  iVar12 = (int)param_2;
  if (bVar1 == 1) {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x24))) != '\0') {
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 4)));
      if (iVar2 == -1) {
        iVar10 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x18)));
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 4))) = 0xffffffff;
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x24))) = 1;
        gs_tasks_kill(param_1,0xffffffffffffffff);
        return 0;
      }
      iVar5 = DispatchByIndex15(0);
      DestroyTaskRecord(param_1);
      uVar3 = countFlaggedSlotsAlt((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x23))),0,iVar2);
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x27))) = uVar3;
      if ((int)(*(uint32_t*)rtguest::G((uint32_t)0x177fdb4 + (uint32_t)(uVar11 * 0xd12) * 4)) < iVar5) {
        uVar9 = uVar11 | iVar2 << 0x18 | 0x200;
      }
      else {
        lVar8 = TestFlaggedSlotBit(0,iVar2,uVar11);
        if (lVar8 == 0) {
          gs_tasks_spawn(param_1,0x268890,uVar11 | iVar2 << 0x18 | 0x100);
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x25))) = 3;
          goto LAB_00269b44;
        }
        uVar9 = uVar11 | iVar2 << 0x18 | 0x300;
      }
      gs_tasks_spawn(param_1,0x268428,uVar9);
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x25))) = 2;
    }
  }
  else if (bVar1 < 2) {
    if (bVar1 != 0) {
      cVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x26)));
      goto LAB_00269b48;
    }
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x25))) = 1;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x26))) = 1;
  }
  else if (bVar1 == 2) {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x24))) == '\0') {
      cVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x26)));
      goto LAB_00269b48;
    }
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x25))) = 0;
  }
  else {
    if (bVar1 != 3) {
      cVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x26)));
      goto LAB_00269b48;
    }
    uVar6 = findFlaggedSlotIndexAlt((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x23))),0,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x27))))
    ;
    uVar7 = DispatchByIndex15(0,uVar6);
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x24))) != '\0') {
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 4))) == 0) {
        SetFlaggedSlotBit(0,uVar6,uVar11);
        SpendTimerBudget(uVar7,uVar11);
        if ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x27))) != '\0') {
          lVar8 = findFlaggedSlotIndexAlt((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x23))),0,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x27))))
          ;
          if (lVar8 != -1) {
            (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x25))) = 0;
            goto LAB_00269b3c;
          }
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x27))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x27))) + -1;
        }
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x25))) = 0;
      }
      else {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x25))) = 0;
      }
    }
LAB_00269b3c:
    DrawScoreAndRemaining(uVar7,param_2);
  }
LAB_00269b44:
  cVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x26)));
LAB_00269b48:
  if (cVar4 != '\0') {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x26))) = 0;
    DestroyTaskRecord(param_1);
    lVar8 = countFlaggedSlotsAlt((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x23))),0,0xffffffffffffffff);
    if (lVar8 < 1) {
      gs_tasks_spawn(param_1,0x269d50,0);
    }
    else {
      gs_tasks_spawn(param_1,0x246250,(uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x27))) << 8 | uVar11 << 0x14);
    }
  }
  return 0;
}


RT_GUEST_FN(0x00269900, guest::shopBodySellMenuTask)

}  // namespace guest
