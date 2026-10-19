// guest:0x0020e708 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateTownBlockTasks(uint32_t param_1, uint32_t param_2, ...) {
  byte bVar1;
  uint uVar2;
  uint uVar3;
uint32_t puVar4;
  long lVar5;
  undefined8 uVar6;
  long lVar7;
  char cVar8;
  uint uVar9;
  int iVar10;
uint32_t ppuVar11;
uint32_t ppuVar12;
uint32_t piVar13;
  int iVar14;
  int iVar15;
  ulong uVar16;
  
  iVar14 = (int)param_1;
  iVar15 = (int)param_2;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 8))) == 0) {
    SetAICarController(0x2508e0);
    cVar8 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x23)));
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x27))) = cVar8;
    if (cVar8 == '\0') {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x27))) = 9;
    }
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x27)));
    cVar8 = (*(int8_t*)rtguest::G((uint32_t)0x2be43f + (uint32_t)((uint32_t)bVar1 * 8) * 1));
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x26))) = cVar8;
    if (cVar8 != '\0') {
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x25))) = (bVar1 != 8) + '\x03';
    }
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x27))) == '\x16') {
      uVar6 = gs_tasks_spawn(param_1,0x218458,(uint32_t)bVar1 << 10 | 0x800021);
      iVar15 = 0;
      SetTaskFlag(uVar6);
      uVar6 = gs_tasks_spawn(param_1,0x218458,(uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x27))) << 10 | 0x800042);
      SetTaskFlag(uVar6);
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x26))) == '\0') {
        cVar8 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x25)));
        goto LAB_0020eb84;
      }
      uVar9 = 0x80;
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x27)));
      while( true ) {
        uVar3 = iVar15 + 3;
        iVar15 = iVar15 + 1;
        uVar2 = (uint32_t)bVar1 << 10 | uVar9;
        uVar9 = uVar9 + 0x20;
        uVar6 = gs_tasks_spawn(param_1,0x218458,uVar2 | uVar3 | 0x800000);
        SetTaskFlag(uVar6);
        if ((int)(uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x26))) <= iVar15) break;
        bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x27)));
      }
    }
    else {
      iVar10 = 0;
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x26))) != '\0') {
        while( true ) {
          lVar5 = CheckUnlockCondition
                            (bVar1,(char)(*(uint8_t*)rtguest::G((uint32_t)0x2be43e + (uint32_t)((uint32_t)bVar1 * 8) * 1)) + iVar10,
                             (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x14))));
          if (lVar5 == 0) {
            uVar6 = gs_tasks_spawn(param_1,0x218458,
                                   (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x27))) << 10 | (iVar10 + 4) * 0x20 |
                                   iVar10 + 1U | 0x800000);
            SetTaskFlag(uVar6);
            bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x26)));
          }
          else {
            bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x26)));
          }
          iVar10 = iVar10 + 1;
          if ((int)(uint32_t)bVar1 <= iVar10) break;
          bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x27)));
        }
        cVar8 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x25)));
        goto LAB_0020eb84;
      }
    }
    cVar8 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x25)));
  }
  else {
    cVar8 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x25)));
    if (cVar8 == '\x02') {
      lVar5 = tileSlotLoaded(param_2,(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x002be43c + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x27))) * 8))));
      if (lVar5 == 0) {
        ::printf((char *)rtguest::G((uint32_t)(0x2ea770u)), (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x002be43c + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x27))) * 8))));
        DestroyTaskRecord(param_1);
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x26))) = 0;
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x25))) = 0;
        cVar8 = '\0';
      }
      else {
        cVar8 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x25)));
      }
    }
    else if (cVar8 == '\0') {
      ppuVar11 = 0x2be440u;
      ppuVar12 = 0x2c22ccu;
      uVar9 = 1;
      do {
        if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)ppuVar11 + 7))) != '\0') &&
           (lVar5 = tileSlotLoaded(param_2,(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(ppuVar11 + (uint32_t)(1) * sizeof(uint32_t))))), lVar5 != 0)) {
          ::printf((char *)rtguest::G((uint32_t)(0x2ea780u)), (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(ppuVar11 + (uint32_t)(1) * sizeof(uint32_t)))));
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x26))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)ppuVar11 + 7)));
          if (uVar9 != (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x27)))) {
            (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x27))) = (char)uVar9;
            lVar5 = 0;
            uVar16 = (uint64_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)ppuVar11 + 7)));
            cVar8 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)ppuVar11 + 6)));
            if (0 < (long)uVar16) {
              piVar13 = 0x0177ea60;
              do {
                iVar15 = (int)cVar8 + (int)lVar5;
                lVar5 = (long)((int)lVar5 + 1);
                lVar7 = MatchSlotPairState(uVar9,iVar15);
                if (lVar7 == 0) {
                  puVar4 = (*(uint32_t*)rtguest::G((uint32_t)ppuVar12)) + iVar15 * 0x10;
                }
                else {
                  puVar4 = (uint32_t)((int)0x177f760 + (int)lVar7 * 0x1c);
                }
                (*(int32_t*)rtguest::G((uint32_t)piVar13)) = (int)puVar4;
                piVar13 = piVar13 + (uint32_t)(1) * sizeof(int32_t);
              } while (lVar5 < (long)uVar16);
            }
            gs_tasks_spawn(param_1,0x20dc50,uVar16 | 0x370400);
          }
          cVar8 = '\x01';
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x25))) = 1;
          goto LAB_0020eb84;
        }
        uVar9 = uVar9 + 1;
        ppuVar11 = ppuVar11 + (uint32_t)(2) * sizeof(uint32_t);
        ppuVar12 = ppuVar12 + (uint32_t)(1) * sizeof(uint32_t);
      } while ((int)uVar9 < 0x16);
      cVar8 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x25)));
    }
    else if ((cVar8 == '\x01') && ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0xf))) == '\0')) {
      iVar10 = 0;
      cVar8 = '\x03' - ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x27))) != '\b');
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x25))) = cVar8;
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x26))) != '\0') {
        bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x27)));
        while( true ) {
          lVar5 = CheckUnlockCondition
                            (bVar1,(char)(*(uint8_t*)rtguest::G((uint32_t)0x2be43e + (uint32_t)((uint32_t)bVar1 * 8) * 1)) + iVar10,
                             (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x14))));
          if (lVar5 == 0) {
            gs_tasks_spawn(param_1,0x218458,
                           (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x27))) << 10 | (iVar10 + 4) * 0x20 | iVar10 + 1U
                           | 0x800000);
            bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x26)));
          }
          else {
            bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x26)));
          }
          iVar10 = iVar10 + 1;
          if ((int)(uint32_t)bVar1 <= iVar10) break;
          bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x27)));
        }
        cVar8 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x25)));
      }
    }
  }
LAB_0020eb84:
  if ((cVar8 == '\x04') &&
     (lVar5 = tileSlotLoaded(param_2,(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x002be43c + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x27))) * 8)))),
     lVar5 != 0)) {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x25))) = 2;
  }
  return 0;
}


RT_GUEST_FN(0x0020e708, guest::UpdateTownBlockTasks)

}  // namespace guest
