// guest:0x0020ed30 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SpawnRaceRivalTasks(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x58) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x5c) = 0x00000000u;
    *(uint32_t*)rtguest::G(rt_sv+0x0) = (uint32_t)(int32_t)param_1;
  }
uint32_t pbVar1;
  bool bVar2;
  byte bVar3;
  char cVar4;
  undefined1 uVar5;
  undefined8 uVar6;
  long lVar7;
uint32_t piVar8;
  uint uVar9;
  int iVar10;
uint32_t puVar11;
uint32_t puVar12;
uint32_t puVar13;
  int iVar14;
  int iVar15;
  uint uVar16;
  uint uVar17;
  uint uVar18;
  int iVar19;
  
  iVar19 = (int)param_2;
  if (param_2 == 0) {
    iVar14 = 0;
  }
  else {
    iVar14 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 1))) - 1;
  }
  uVar18 = 0;
  if ((param_4 != 0) && (param_3 == 0)) {
    iVar14 = 1;
  }
  uVar6 = RT_CALLRA(0x20edb4, guest::gs_tasks_spawn(param_1,0x218458,iVar14 << 10 | 0x20000));
  RT_CALLRA(0x20edc0, guest::SetTaskFlag(uVar6));
  (*(uint8_t*)rtguest::G(0x178700a)) = (uint32_t)iVar14;
  iVar15 = iVar14 + -1;
  (*(uint32_t*)rtguest::G(0x178700b)) = 2;
  if (-1 < iVar15) {
    uVar16 = 1;
    if (param_4 != 0) {
      uVar6 = RT_CALLRA(0x20ee00, guest::gs_tasks_spawn(param_1,0x218458,iVar15 * 0x400 | 0x430061));
      uVar16 = 2;
      RT_CALLRA(0x20ee0c, guest::SetTaskFlag(uVar6));
      (*(uint32_t*)rtguest::G(0x178700e)) = (uint32_t)iVar15;
      iVar15 = iVar14 + -2;
      (*(uint32_t*)rtguest::G(0x178700f)) = 0x43;
      if (iVar15 < 0) {
        (*(uint32_t*)rtguest::G(0x178700f)) = 0x43;
        return 0;
      }
    }
    if (param_3 != 0) {
      bVar2 = (*(int8_t*)rtguest::G(0x177fdd4)) != '\0';
      if (bVar2) {
        uVar6 = RT_CALLRA(0x20ee50, guest::gs_tasks_spawn(param_1,0x218458,uVar16 | 0x900020));
        RT_CALLRA(0x20ee58, guest::SetTaskFlag(uVar6));
        uVar5 = (*(uint32_t*)rtguest::G(0x177fdd5));
        cVar4 = (*(int8_t*)rtguest::G(0x177fdd4));
        iVar14 = uVar16 * 4;
        (*(uint32_t*)rtguest::G((uint32_t)0x178700b + (uint32_t)(iVar14) * 4)) = 0x90;
        (*(uint32_t*)rtguest::G((uint32_t)0x1787008 + (uint32_t)(iVar14) * 4)) = cVar4;
        uVar16 = uVar16 + 1;
        (*(uint32_t*)rtguest::G((uint32_t)0x1787009 + (uint32_t)(iVar14) * 4)) = uVar5;
        (*(uint8_t*)rtguest::G((uint32_t)0x178700a + (uint32_t)(iVar14) * 1)) = 0;
      }
      uVar18 = (uint32_t)bVar2;
      if ((param_4 != 0) && ((*(uint32_t*)rtguest::G(0x178321c)) != '\0')) {
        uVar6 = RT_CALLRA(0x20eec4, guest::gs_tasks_spawn(param_1,0x218458,uVar18 << 10 | uVar16 | 0xd00080));
        RT_CALLRA(0x20eecc, guest::SetTaskFlag(uVar6));
        uVar5 = (*(uint32_t*)rtguest::G(0x178321d));
        cVar4 = (*(uint32_t*)rtguest::G(0x178321c));
        iVar14 = uVar16 * 4;
        (*(uint32_t*)rtguest::G((uint32_t)0x178700b + (uint32_t)(iVar14) * 4)) = 0xd0;
        uVar16 = uVar16 + 1;
        (*(uint32_t*)rtguest::G((uint32_t)0x1787008 + (uint32_t)(iVar14) * 4)) = cVar4;
        (*(uint32_t*)rtguest::G((uint32_t)0x1787009 + (uint32_t)(iVar14) * 4)) = uVar5;
        (*(uint8_t*)rtguest::G((uint32_t)0x178700a + (uint32_t)(iVar14) * 1)) = bVar2;
        uVar18 = uVar18 + 1;
      }
      if ((*(int8_t*)rtguest::G(0x177fdd6)) != '\0') {
        uVar6 = RT_CALLRA(0x20ef40, guest::gs_tasks_spawn(param_1,0x218458,uVar18 << 10 | uVar16 | 0xa00040));
        RT_CALLRA(0x20ef48, guest::SetTaskFlag(uVar6));
        uVar5 = (*(uint32_t*)rtguest::G(0x177fdd7));
        cVar4 = (*(int8_t*)rtguest::G(0x177fdd6));
        iVar14 = uVar16 * 4;
        uVar16 = uVar16 + 1;
        (*(uint32_t*)rtguest::G((uint32_t)0x178700b + (uint32_t)(iVar14) * 4)) = 0xa0;
        (*(uint32_t*)rtguest::G((uint32_t)0x1787008 + (uint32_t)(iVar14) * 4)) = cVar4;
        (*(uint32_t*)rtguest::G((uint32_t)0x1787009 + (uint32_t)(iVar14) * 4)) = uVar5;
        (*(uint8_t*)rtguest::G((uint32_t)0x178700a + (uint32_t)(iVar14) * 1)) = (char)uVar18;
        uVar18 = uVar18 + 1;
      }
      if ((param_4 != 0) && ((*(uint32_t*)rtguest::G(0x178321e)) != '\0')) {
        uVar6 = RT_CALLRA(0x20efbc, guest::gs_tasks_spawn(param_1,0x218458,uVar18 << 10 | uVar16 | 0xe000a0));
        RT_CALLRA(0x20efc4, guest::SetTaskFlag(uVar6));
        uVar5 = (*(uint32_t*)rtguest::G(0x178321f));
        cVar4 = (*(uint32_t*)rtguest::G(0x178321e));
        iVar14 = uVar16 * 4;
        uVar16 = uVar16 + 1;
        (*(uint32_t*)rtguest::G((uint32_t)0x178700b + (uint32_t)(iVar14) * 4)) = 0xe0;
        (*(uint32_t*)rtguest::G((uint32_t)0x1787008 + (uint32_t)(iVar14) * 4)) = cVar4;
        (*(uint32_t*)rtguest::G((uint32_t)0x1787009 + (uint32_t)(iVar14) * 4)) = uVar5;
        (*(uint8_t*)rtguest::G((uint32_t)0x178700a + (uint32_t)(iVar14) * 1)) = (char)uVar18;
        uVar18 = uVar18 + 1;
      }
    }
    if ((param_2 != 0) && (param_4 == 0)) {
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 3))) == '\x03') {
        iVar14 = 0;
        if (0 < (int)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 1))) - 1)) {
          iVar15 = uVar16 * 4;
          uVar17 = 0x60;
          puVar12 = 0x01787008 + iVar15;
          do {
            iVar10 = (*(int32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 4)))));
            (*(uint32_t*)rtguest::G((uint32_t)0x178700b + (uint32_t)(iVar15) * 4)) = 0x80;
            puVar13 = (uint32_t)(iVar10 + iVar14 * 2);
            (*(uint8_t*)rtguest::G((uint32_t)puVar12)) = (*(uint8_t*)rtguest::G((uint32_t)puVar13));
            (*(uint32_t*)rtguest::G((uint32_t)0x1787009 + (uint32_t)(iVar15) * 4)) = (*(uint8_t*)rtguest::G((uint32_t)puVar13 + (uint32_t)(1) * sizeof(uint8_t)));
            if (uVar18 == 6) {
              (*(uint8_t*)rtguest::G((uint32_t)0x178700a + (uint32_t)(iVar15) * 1)) = 3;
            }
            else if ((int)uVar18 < 7) {
              if (uVar18 == 3) {
                (*(uint8_t*)rtguest::G((uint32_t)0x178700a + (uint32_t)(iVar15) * 1)) = 6;
              }
              else {
                (*(uint8_t*)rtguest::G((uint32_t)puVar12 + (uint32_t)(2) * sizeof(uint8_t))) = (char)uVar18;
              }
            }
            else if (uVar18 == 9) {
              (*(uint8_t*)rtguest::G((uint32_t)0x178700a + (uint32_t)(iVar15) * 1)) = 0xc;
            }
            else if (uVar18 == 0xc) {
              (*(uint8_t*)rtguest::G((uint32_t)0x178700a + (uint32_t)(iVar15) * 1)) = 9;
            }
            else {
              (*(uint8_t*)rtguest::G((uint32_t)puVar12 + (uint32_t)(2) * sizeof(uint8_t))) = (char)uVar18;
            }
            pbVar1 = puVar12 + 2;
            puVar12 = puVar12 + 4;
            uVar9 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar1)) << 10 | uVar17 | uVar16;
            uVar16 = uVar16 + 1;
            uVar18 = uVar18 + 1;
            uVar6 = RT_CALLRA(0x20f128, guest::gs_tasks_spawn(param_1,0x218458,uVar9 | 0x800000));
            iVar15 = iVar15 + 4;
            RT_CALLRA(0x20f134, guest::SetTaskFlag(uVar6));
            if (uVar16 == (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 1)))) {
              return 0;
            }
            iVar14 = iVar14 + 1;
            uVar17 = uVar17 + 0x20;
          } while (iVar14 < (int)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 1))) - 1));
        }
      }
      else {
        iVar14 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 1))) - 2;
        puVar12 = 0x01787008 + uVar16 * 4;
        uVar17 = iVar14 * 0x20 + 0x60;
        puVar13 = 0x01787009 + uVar16 * 4;
        piVar8 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 4)));
        while( true ) {
          iVar10 = iVar14 * 2;
          iVar14 = iVar14 + -1;
          puVar11 = (uint32_t)((*(int32_t*)rtguest::G((uint32_t)piVar8)) + iVar10);
          lVar7 = RT_CALLRA(0x20f1a4, guest::MatchSlotPairState((*(uint8_t*)rtguest::G((uint32_t)puVar11)),(*(uint8_t*)rtguest::G((uint32_t)puVar11 + (uint32_t)(1) * sizeof(uint8_t)))));
          uVar9 = iVar15 << 10 | uVar17;
          uVar17 = uVar17 - 0x20;
          uVar9 = uVar9 | uVar16;
          if (lVar7 == 0) {
            uVar16 = uVar16 + 1;
            uVar6 = RT_CALLRA(0x20f1d4, guest::gs_tasks_spawn(param_1,0x218458,uVar9 | 0x800000));
            RT_CALLRA(0x20f1dc, guest::SetTaskFlag(uVar6));
            (*(uint8_t*)rtguest::G((uint32_t)puVar13 + (uint32_t)(2) * sizeof(uint8_t))) = 0x80;
            (*(uint8_t*)rtguest::G((uint32_t)puVar12)) = (*(uint8_t*)rtguest::G((uint32_t)puVar11));
            (*(uint8_t*)rtguest::G((uint32_t)puVar13)) = (*(uint8_t*)rtguest::G((uint32_t)puVar11 + (uint32_t)(1) * sizeof(uint8_t)));
            puVar13 = puVar13 + 4;
            (*(uint8_t*)rtguest::G((uint32_t)puVar12 + (uint32_t)(2) * sizeof(uint8_t))) = (char)iVar15;
            iVar15 = iVar15 + -1;
            bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 1)));
            uVar9 = (uint32_t)bVar3;
            puVar12 = puVar12 + 4;
            if (uVar16 == uVar9) {
              return 0;
            }
          }
          else {
            bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 1)));
            uVar9 = (uint32_t)bVar3;
          }
          if (iVar14 <= (int)(bVar3 - 8)) break;
          piVar8 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 4)));
        }
        iVar14 = 0;
        if (0 < (int)(uVar9 - 1)) {
          uVar17 = 0x60;
          puVar12 = 0x01787008 + uVar16 * 4;
          puVar13 = 0x01787009 + uVar16 * 4;
          piVar8 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 4)));
          while( true ) {
            iVar15 = iVar14 * 2;
            iVar14 = iVar14 + 1;
            puVar11 = (uint32_t)((*(int32_t*)rtguest::G((uint32_t)piVar8)) + iVar15);
            lVar7 = RT_CALLRA(0x20f274, guest::MatchSlotPairState((*(uint8_t*)rtguest::G((uint32_t)puVar11)),(*(uint8_t*)rtguest::G((uint32_t)puVar11 + (uint32_t)(1) * sizeof(uint8_t)))));
            uVar9 = uVar18 << 10 | uVar17;
            uVar17 = uVar17 + 0x20;
            uVar9 = uVar9 | uVar16;
            if (lVar7 == 0) {
              uVar16 = uVar16 + 1;
              uVar6 = RT_CALLRA(0x20f2a4, guest::gs_tasks_spawn(param_1,0x218458,uVar9 | 0x800000));
              RT_CALLRA(0x20f2ac, guest::SetTaskFlag(uVar6));
              (*(uint8_t*)rtguest::G((uint32_t)puVar13 + (uint32_t)(2) * sizeof(uint8_t))) = 0x80;
              (*(uint8_t*)rtguest::G((uint32_t)puVar12)) = (*(uint8_t*)rtguest::G((uint32_t)puVar11));
              (*(uint8_t*)rtguest::G((uint32_t)puVar13)) = (*(uint8_t*)rtguest::G((uint32_t)puVar11 + (uint32_t)(1) * sizeof(uint8_t)));
              puVar13 = puVar13 + 4;
              (*(uint8_t*)rtguest::G((uint32_t)puVar12 + (uint32_t)(2) * sizeof(uint8_t))) = (char)uVar18;
              uVar18 = uVar18 + 1;
              bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 1)));
              puVar12 = puVar12 + 4;
              if (uVar16 == bVar3) {
                return 0;
              }
            }
            else {
              bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 1)));
            }
            if ((int)(bVar3 - 1) <= iVar14) break;
            piVar8 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar19 + 4)));
          }
          return 0;
        }
      }
    }
  }
  return 0;
}


RT_GUEST_FN(0x0020ed30, guest::SpawnRaceRivalTasks)

}  // namespace guest
