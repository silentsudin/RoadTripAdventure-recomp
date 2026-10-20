// guest:0x00261638 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t rouletteBetAmountTask(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(48);
  byte bVar1;
  ushort uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
uint32_t puVar8;
  int iVar9;
  int iVar10;
uint32_t puVar11;
  
  iVar9 = (int)param_1;
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x1c)));
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8))) == 0) {
    return 0;
  }
  bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25)));
  puVar11 = (uint32_t)param_2;
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if ((bVar1 == 2) && ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24))) != '\0')) {
        if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 4))) != 1) {
          SpendTimerBudget((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8010))),0,param_3,param_4,0);
          iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x18)));
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = 0;
          gs_tasks_kill(param_1);
          return 0;
        }
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) = 1;
      }
      goto LAB_00261970;
    }
    if (bVar1 != 0) goto LAB_00261970;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8010))) = 0;
    iVar4 = (*(int32_t*)rtguest::G(0x177fdb4));
    if (10000 < (*(int32_t*)rtguest::G(0x177fdb4))) {
      iVar4 = 10000;
    }
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8014))) = iVar4;
    iVar4 = CountDecimalDigits(iVar4);  // 2616bc movz loads clamped value into $4
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x801c))) = 0;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8018))) = iVar4 + -1;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) = 1;
  }
  iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x801c)));
  iVar10 = 10;
  iVar6 = iVar4;
  if (0 < iVar4) {
    do {
      iVar6 = iVar6 + -1;
      iVar10 = iVar10 * 10;
    } while (iVar6 != 0);
  }
  iVar5 = NavigateGridSelection
                    (param_2,((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8018))) - iVar4) + -1,(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8018))),1,0,1)
  ;
  iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8010)));
  iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8014)));
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x801c))) = ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8018))) - iVar5) + -1;
  iVar5 = iVar4;
  if (iVar4 == iVar6) {
    if (((*(uint64_t*)rtguest::G((uint32_t)puVar11 + (uint32_t)(6) * sizeof(uint64_t))) & 0x1000) != 0) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8010))) = 0;
      iVar5 = 0;
    }
  }
  else {
    iVar7 = iVar4 + iVar10;
    if ((((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar11 + 0x34))) & 0x1000) != 0) &&
       ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8010))) = iVar7, iVar5 = iVar7, iVar6 < iVar7)) {
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8010))) = iVar6;
      iVar5 = iVar6;
    }
  }
  if (iVar5 == 0) {
    if (((*(uint64_t*)rtguest::G((uint32_t)puVar11 + (uint32_t)(6) * sizeof(uint64_t))) & 0x4000) != 0) {
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8010))) = iVar6;
      iVar5 = iVar6;
    }
  }
  else {
    iVar10 = iVar5 - iVar10;
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar11 + 0x34))) & 0x4000) != 0) {
      if (iVar10 < 0) {
        iVar10 = 0;
      }
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8010))) = iVar10;
      iVar5 = iVar10;
    }
  }
  if (iVar5 == iVar4) {
    uVar2 = (uint32_t)(*(uint64_t*)rtguest::G((uint32_t)puVar11 + (uint32_t)(6) * sizeof(uint64_t)));
  }
  else {
    PlayUiSound(0x1d);
    uVar2 = (uint32_t)(*(uint64_t*)rtguest::G((uint32_t)puVar11 + (uint32_t)(6) * sizeof(uint64_t)));
  }
  if ((uVar2 & 0x40) == 0) {
    if ((uVar2 & 0x10) != 0) {
      PlayUiSound(0x1b);
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = 0xffffffff;
      gs_tasks_kill(param_1);
      return 0;
    }
  }
  else {
    PlayUiSound(0x1a);
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8010))) == 0) {
      gs_tasks_spawn(param_1,2500000,0);
    }
    else {
      gs_tasks_spawn(param_1,0x261508,0);
    }
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) = 2;
  }
LAB_00261970:
  uiDrawPanel9(0x78,0x3e,400,0x24,0);
  uiDrawText(0x78,0x3e,0x3292f0,0);
  uiDrawText(0x158,0x4d,0x177fd9f,0);
  uiDrawNumberFixed(0x158,0x9c,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8010))),0,(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8018))) + 1);
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) != '\x02') {
    uiDrawIcon20((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x801c))) * -0x10 + 0x136,0xb2 - ((uint32_t)((*(uint64_t*)rtguest::G((uint32_t)puVar11)) >> 1) & 7),6);
  }
  puVar8 = (uint32_t)
           ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8010))) *
            ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002d9980 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8008))) * 0x30))) + -1) + (*(int32_t*)rtguest::G(0x177fdb4)));
  if (999999 < (int)puVar8) {
    puVar8 = 0x000f423f;
  }
  DrawScoreAndRemaining((*(int32_t*)rtguest::G(0x177fdb4)) - (int)puVar8,param_2);
  return 0;
}


RT_GUEST_FN(0x00261638, guest::rouletteBetAmountTask)

}  // namespace guest
