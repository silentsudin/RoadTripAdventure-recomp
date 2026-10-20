// guest:0x0026be50 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ProcessLocationArrival(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  byte bVar1;
  int iVar2;
  char cVar3;
  undefined8 uVar4;
  long lVar5;
  undefined1 uVar6;
uint32_t piVar7;
uint32_t puVar8;
  uint uVar9;
  int iVar10;
uint32_t puVar11;
  int iVar12;
  int iVar13;
  int iVar14;
  int iVar15;
uint32_t auStack_440 = rtguest::allocFrame(256);
uint32_t auStack_340 = rtguest::allocFrame(256);
uint32_t auStack_240 = rtguest::allocFrame(256);
uint32_t auStack_140 = rtguest::allocFrame(256);
  
  iVar15 = (int)param_1;
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x28)));
  iVar10 = (int)param_2;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 8))) == 0) {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x23))) == '\0') {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x25))) = 1;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x21))) = 0;
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x1c))) != 0) {
        SpawnTownDialogueTasks(param_1,param_2);  // $4/$5 intact since entry (no writes 26be60..26bea8)
        return 0;
      }
      StartTownExplorationTask(param_1,param_2);  // beqz path, $4/$5 intact; 26c428 reloads prove clobber AFTER spawn
      return 0;
    }
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x26))) = 0;
    uVar4 = uiMenuCreate();
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x1c))) == 0) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x26))) = (*(uint8_t*)rtguest::G((uint32_t)0x2be43e + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x23))) * 8) * 1));
    }
    else {
      piVar7 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002a7ed0 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x23))) * 4)));
      if ((*(int32_t*)rtguest::G((uint32_t)piVar7)) != 0) {
        do {
          piVar7 = piVar7 + 0x4;  // T* element step
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x26))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x26))) + '\x01';
        } while ((*(int32_t*)rtguest::G((uint32_t)piVar7)) != 0);
        bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x26)));
        goto LAB_0026bf48;
      }
    }
    bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x26)));
LAB_0026bf48:
    uVar9 = (uint32_t)bVar1;
    if (uVar9 != 0) {
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002a7ed0 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x23))) * 4)));
      iVar15 = 0;
      do {
        iVar10 = iVar15 + 1;
        (*(uint32_t*)rtguest::G((uint32_t)auStack_340 + (uint32_t)(iVar15) * 4)) = (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 * 4 + iVar2)))));
        iVar15 = iVar10;
      } while (iVar10 < (int)uVar9);
    }
    LayoutTextItems(uVar4,auStack_340,uVar9,0xf);
    return 0;
  }
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x24))) == '\0') goto LAB_0026c230;
  iVar14 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 4)));
  if (iVar14 == 6) {
LAB_0026bfd4:
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = 0xffffffff;
    gs_tasks_kill();
    return 0;
  }
  if (iVar14 == 5) {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x23))) != '\0') {
      gs_tasks_spawn(param_1,0x26d008,0);
      cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x25)));
      goto LAB_0026c234;
    }
    goto LAB_0026bfd4;
  }
  if (iVar14 == 0xb) {
    DestroyTaskRecord();
    SpawnTownDialogueTasks(param_1,param_2);
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x23))) == '\0') goto LAB_0026c230;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x26))) = 0;
    uVar4 = uiMenuCreate(param_1);
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x1c))) == 0) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x26))) = (*(uint8_t*)rtguest::G((uint32_t)0x2be43e + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x23))) * 8) * 1));
LAB_0026c0ac:
      bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x26)));
    }
    else {
      piVar7 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002a7ed0 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x23))) * 4)));
      if ((*(int32_t*)rtguest::G((uint32_t)piVar7)) == 0) goto LAB_0026c0ac;
      do {
        piVar7 = piVar7 + 0x4;  // T* element step
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x26))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x26))) + '\x01';
      } while ((*(int32_t*)rtguest::G((uint32_t)piVar7)) != 0);
      bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x26)));
    }
    uVar9 = (uint32_t)bVar1;
    if (uVar9 != 0) {
      iVar14 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002a7ed0 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x23))) * 4)));
      iVar12 = 0;
      do {
        iVar13 = iVar12 + 1;
        (*(uint32_t*)rtguest::G((uint32_t)auStack_240 + (uint32_t)(iVar12) * 4)) = (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 * 4 + iVar14)))));
        iVar12 = iVar13;
      } while (iVar13 < (int)uVar9);
    }
    puVar8 = auStack_240;
  }
  else {
    if (iVar14 != 0xc) {
      if (iVar14 == 4) {
        DestroyTaskRecord();
        SpawnTownDialogueTasks(param_1,param_2);
        cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x25)));
        goto LAB_0026c234;
      }
      if (iVar14 != -1) {
        cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x25)));
        goto LAB_0026c234;
      }
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x25))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x50))) = 0x8033cc66;
LAB_0026c230:
      cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x25)));
      goto LAB_0026c234;
    }
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x23))) == '\0') {
      cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x25)));
      goto LAB_0026c234;
    }
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x26))) = 0;
    uVar4 = uiMenuCreate();
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x1c))) == 0) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x26))) = (*(uint8_t*)rtguest::G((uint32_t)0x2be43e + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x23))) * 8) * 1));
LAB_0026c18c:
      bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x26)));
    }
    else {
      piVar7 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002a7ed0 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x23))) * 4)));
      if ((*(int32_t*)rtguest::G((uint32_t)piVar7)) == 0) goto LAB_0026c18c;
      do {
        piVar7 = piVar7 + 0x4;  // T* element step
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x26))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x26))) + '\x01';
      } while ((*(int32_t*)rtguest::G((uint32_t)piVar7)) != 0);
      bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x26)));
    }
    uVar9 = (uint32_t)bVar1;
    if (uVar9 != 0) {
      iVar14 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002a7ed0 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x23))) * 4)));
      iVar12 = 0;
      do {
        iVar13 = iVar12 + 1;
        (*(uint32_t*)rtguest::G((uint32_t)auStack_140 + (uint32_t)(iVar12) * 4)) = (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 * 4 + iVar14)))));
        iVar12 = iVar13;
      } while (iVar13 < (int)uVar9);
    }
    puVar8 = auStack_140;
  }
  LayoutTextItems(uVar4,puVar8,uVar9,0xf);
  cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x25)));
LAB_0026c234:
  if (cVar3 != '\0') {
    return 0;
  }
  bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x26)));
  if (bVar1 != 0) {
    iVar14 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002a7ed0 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x23))) * 4)));
    iVar12 = 0;
    do {
      iVar13 = iVar12 + 1;
      (*(uint32_t*)rtguest::G((uint32_t)auStack_440 + (uint32_t)(iVar12) * 4)) = (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 * 4 + iVar14)))));
      iVar12 = iVar13;
    } while (iVar13 < (int)(uint32_t)bVar1);
  }
  iVar14 = 0;
  if ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xb))) != '\0') {
    puVar11 = (uint32_t)(iVar2 + 0x50);
    do {
      lVar5 = TestStampFlag((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x23))),iVar14 + (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x10))));
      uVar6 = 0x10;
      if (lVar5 == 0) {
        uVar6 = 0x12;
      }
      (*(uint8_t*)rtguest::G((uint32_t)puVar11)) = uVar6;
      lVar5 = CheckUnlockCondition
                        ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x23))),iVar14 + (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x10))),
                         (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x14))));
      if (lVar5 != 0) {
        (*(uint8_t*)rtguest::G((uint32_t)puVar11)) = 0x11;
      }
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x23))) == '\t') {
        lVar5 = IsMapEntryEnabled(iVar14 + (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x10))));
        if (lVar5 == 0) {
          (*(uint8_t*)rtguest::G((uint32_t)puVar11)) = 0x11;
        }
        bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xb)));
      }
      else {
        bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xb)));
      }
      iVar14 = iVar14 + 1;
      puVar11 = puVar11 + 1;
    } while (iVar14 < (int)(uint32_t)bVar1);
  }
  lVar5 = uiMenuWindow(iVar2,param_2,auStack_440);
  if (lVar5 == 0) {
    return 0;
  }
  if (lVar5 == -1) {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = 0xffffffff;
    gs_tasks_kill(param_1);
    return 0;
  }
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x25))) = 1;
  bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xd)));
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x21))) = bVar1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x1c))) == 0) {
    StartTownExplorationTask(param_1,param_2);
    return 0;
  }
  cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x23)));
  if ((long)(uint32_t)bVar1 < (long)(char)(*(uint8_t*)rtguest::G((uint32_t)0x2be43e + (uint32_t)(cVar3 * 8) * 1))) {
    SpawnTownDialogueTasks(param_1,param_2);
    return 0;
  }
  lVar5 = MatchSlotPairState(cVar3,(uint32_t)bVar1);
  if (lVar5 == 0) {
    (*(int32_t*)rtguest::G(0x177ea5c)) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x2c22c8 + cVar3 * 4))) + (uint32_t)bVar1 * 0x10;
  }
  else {
    (*(int32_t*)rtguest::G(0x177ea5c)) = (int)0x177f760 + (int)lVar5 * 0x1c;
  }
  gs_tasks_spawn(param_1,0x20dc50,0x70301);
  gs_tasks_spawn(param_1,0x22c518,1);
  return 0;
}


RT_GUEST_FN(0x0026be50, guest::ProcessLocationArrival)

}  // namespace guest
