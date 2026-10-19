// guest:0x0022ddd0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t raceHudTask(uint32_t param_1, uint32_t param_2, ...) {
  char cVar1;
  undefined4 uVar2;
  byte bVar3;
  byte bVar4;
  undefined8 uVar5;
  uint uVar6;
uint32_t puVar7;
  ulong uVar8;
uint32_t puVar9;
  undefined1 uVar10;
  bool bVar11;
  undefined8 uVar12;
uint32_t pbVar13;
uint32_t puVar14;
  int iVar15;
uint32_t puVar16;
  int iVar17;
  int iVar18;
  int iVar19;
  
  iVar15 = (int)param_1;
  iVar17 = (int)param_2;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 8))) == 0) {
    ResetRecordTables(param_2);
    uVar5 = getExtraPacketArea(0x8cb0);
    iVar18 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x10)));
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x28))) = (int)uVar5;
    if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 3))) & 8) == 0) {
LAB_0022de60:
      uVar10 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 1)));
    }
    else if ((*(int8_t*)rtguest::G(0x177fdd6)) == '\0') {
      uVar10 = 2;
    }
    else {
      if ((*(uint32_t*)rtguest::G(0x178321e)) != '\0') goto LAB_0022de60;
      uVar10 = 2;
    }
    raceHudInit(uVar5,(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 10))) >> 1 & 1,uVar10);
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x20))) == '\x1a') {
      uVar8 = 0xa170000;
    }
    else {
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x22)));
      if (cVar1 < '\x12') {
        gs_music_play_bgm((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x29f5b0 + cVar1 * 4))));
        goto LAB_0022dee4;
      }
      if (cVar1 != '\x1c') {
        gs_music_play_bgm(0xa0d0201);
      }
      uVar8 = 0xa160000;
    }
    gs_music_play_bgm(uVar8 | 0x201);
LAB_0022dee4:
    gs_tasks_spawn(param_1,0x22dae8,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x28))));
    if ((((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x10))) + 3))) & 8) == 0) ||
       ((uVar6 = 2, (*(int8_t*)rtguest::G(0x177fdd6)) != '\0' && ((*(uint32_t*)rtguest::G(0x178321e)) != '\0')))) {
      uVar6 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x10))) + 1)));
    }
    if (uVar6 == 0) {
      bVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x25)));
    }
    else {
      pbVar13 = 0x0178700b;
      bVar3 = (*(uint32_t*)rtguest::G(0x178700b));
      while( true ) {
        pbVar13 = pbVar13 + 4;
        if ((bVar3 & 0x73) != 0) {
          (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x25))) =
               (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x25))) |
               (uint32_t)(1 << ((int)(bVar3 & 0x30) >> 4) + ((int)(bVar3 & 0x40) >> 6) * 3);
        }
        uVar6 = uVar6 - 1;
        if (uVar6 == 0) break;
        bVar3 = (*(int8_t*)rtguest::G((uint32_t)pbVar13));
      }
      bVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x25)));
    }
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x25))) = bVar3 ^ 0x3f;
    if (1 < (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x22))) - 0xf) {
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x22))) != 0x26) {
        return 0;
      }
      gs_tasks_spawn(param_1,0x263c38,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x1c))));
    }
    gs_tasks_spawn(param_1,0x22dc68,0);
  }
  uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x28)));
  SetDashConfig(uVar2,0,0);
  SetEngineConfig(uVar2,0,0);
  bVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x25)));
  bVar4 = bVar3 & 1;
  if (0x3e < bVar3) goto LAB_0022e290;
  bVar4 = bVar3 & 1;
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x28))) & 0x8000) != 0) goto LAB_0022e290;
  cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x22)));
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x28))) & 0x44) == 4) {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x20))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x20))) + 1;
  }
  iVar18 = 0x17;
  puVar14 = 0x177ac50;
  do {
    if ((((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(puVar14 + 0x33))) != 0) && (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(puVar14 + 0x33))) & 0x200) == 0)) &&
       (UpdateZoneFromRects(param_1,puVar14,param_2,cVar1 * 0x30 + 0x2a8198),
       (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar14 + 0x19a))) != '\0')) {
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar14 + 0x4c))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar14 + 0x4c))) + 1;
    }
    iVar18 = iVar18 + -1;
    puVar14 = puVar14 + 0x4e;
  } while (-1 < iVar18);
  if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x25))) & 9) == 9) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x28))) | 8;
  }
  RankCarsByProgress((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x26))));
  if ((((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x25))) & 9) == 9) &&
     (((((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x30))) | (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x32)))) & 0x810) != 0 ||
      (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 10))) & 2) != 0)))) {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x25))) = 0x3f;
  }
  if (((*(uint32_t*)rtguest::G(0x177ae98)) < 0x3d) || (((*(uint16_t*)rtguest::G(0x177ade8)) & 0x200) != 0)) {
    raceHudUpdate(uVar2,(*(uint16_t*)rtguest::G(0x177ade8)) & 1,0);
    bVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 10)));
  }
  else {
    raceHudUpdate(uVar2,(*(uint16_t*)rtguest::G(0x177ade8)) & 1,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 8))) & 0x10);
    bVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 10)));
  }
  if ((bVar3 & 2) == 0) {
LAB_0022e220:
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x20)));
  }
  else {
    if (((*(uint32_t*)rtguest::G(0x177b108)) < 0x3d) || (((*(uint32_t*)rtguest::G(0x177b058)) & 0x200) != 0)) {
      raceHudUpdate(uVar2,(*(uint32_t*)rtguest::G(0x177b058)) & 1,0);
      goto LAB_0022e220;
    }
    raceHudUpdate(uVar2,(*(uint32_t*)rtguest::G(0x177b058)) & 1,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 8))) & 0x10);
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x20)));
  }
  if (cVar1 == '\x18') {
    SetDashConfig(uVar2,0,(*(uint16_t*)rtguest::G(0x177ade8)) >> 10 & 1);
    bVar11 = false;
    if ((CONCAT62((*(int8_t*)rtguest::G(0x177ae9a)),(*(uint32_t*)rtguest::G(0x177ae98))) & 0xffffff) == 0x670000) {
      bVar11 = ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x24))) & 0xff000100) == 0;
    }
    SetEngineConfig(uVar2,0,bVar11);
    bVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x25)));
  }
  else {
    bVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x25)));
  }
  bVar4 = bVar3 & 1;
LAB_0022e290:
  if (bVar4 != 0) {
    SetBumperConfig(uVar2,0,1);
    bVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x25)));
  }
  if ((((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 10))) & 2) != 0) && ((bVar3 & 8) != 0)) {
    SetBumperConfig(uVar2,1,1);
    bVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x25)));
  }
  if (bVar3 == 0x3f) {
    uiFade((int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 2))) << 0x18);
    if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 2))) < 0x80) {
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 2))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 2))) + 1;
    }
    else {
      puVar9 = 0x0178700a;
      puVar7 = 0x0177ae97;
      iVar18 = 0;
      do {
        uVar10 = (*(uint8_t*)rtguest::G((uint32_t)puVar7));
        puVar7 = puVar7 + 0x270;
        iVar18 = iVar18 + 1;
        (*(uint8_t*)rtguest::G((uint32_t)puVar9)) = uVar10;
        puVar9 = puVar9 + 4;
      } while (iVar18 < 0x18);
      iVar18 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x18)));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 4))) = 0xffffffff;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar18 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0xc))) = (*(uint8_t*)rtguest::G(0x22ecb8));
    }
    bVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x27)));
  }
  else {
    bVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x27)));
  }
  if (bVar3 != 0) {
    bVar3 = bVar3 - 1;
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x27))) = bVar3;
  }
  SetTailConfig(uVar2,0,bVar3 & 8);
  InitHudConfig((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x28))),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x20))));
  iVar15 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x10)));
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 2))) < '\x02') {
    bVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 1)));
  }
  else {
    DrawRecordTableRows(0x1787068,
                        0x017807a0 +
                        ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x22))) * 0xc + (int)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x22)))) * 8,
                        0x177ac50,uVar2);
    iVar15 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x10)));
    bVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 1)));
  }
  if (bVar3 < 2) {
    uVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x28)));
  }
  else {
    iVar18 = 0;
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 1))) != '\0') {
      puVar16 = 0x0177ae90;
      iVar19 = 0x240;
      do {
        bVar3 = (*(uint32_t*)rtguest::G((uint32_t)0x178700b + (uint32_t)(iVar18 * 4) * 4)) & 0x73;
        if (bVar3 == 0x20) {
          uVar10 = (*(uint8_t*)rtguest::G((uint32_t)puVar16 + (uint32_t)(7) * sizeof(uint8_t)));
          uVar5 = 0;
          uVar12 = 2;
LAB_0022e540:
          SetSlotConfig(uVar2,uVar5,uVar12,uVar10);
          iVar15 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x10)));
LAB_0022e564:
          uVar6 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 1)));
        }
        else {
          if (bVar3 < 0x21) {
            if (bVar3 != 2) {
              if (bVar3 != 0x10) {
                uVar6 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 1)));
                goto LAB_0022e568;
              }
              uVar10 = (*(uint8_t*)rtguest::G((uint32_t)puVar16 + (uint32_t)(7) * sizeof(uint8_t)));
              uVar5 = 0;
              goto LAB_0022e538;
            }
            uVar10 = (*(uint8_t*)rtguest::G((uint32_t)puVar16 + (uint32_t)(7) * sizeof(uint8_t)));
            uVar5 = 0;
            uVar12 = 0;
            goto LAB_0022e540;
          }
          if (bVar3 == 0x50) {
            uVar10 = (*(uint8_t*)rtguest::G((uint32_t)puVar16 + (uint32_t)(7) * sizeof(uint8_t)));
            uVar5 = 1;
LAB_0022e538:
            uVar12 = 1;
          }
          if (0x50 < bVar3) {
            if (bVar3 != 0x60) {
              uVar6 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 1)));
            }
            SetSlotConfig(uVar2,1,2,(*(uint8_t*)rtguest::G((uint32_t)puVar16 + (uint32_t)(7) * sizeof(uint8_t))));
            iVar15 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x10)));
            goto LAB_0022e564;
          }
          if (bVar3 == 0x43) {
            SetSlotConfig(uVar2,1,0,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x177ac50 + iVar19 + 7))));
            iVar15 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x10)));
            if ('\x01' < (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 2)))) {
              DrawRecordTableRows(0x1787098,
                                  ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x22))) * 0xc + (int)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x22)))) *
                                  8 + 0x1783be8,0x177aec0,uVar2);
              iVar15 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x10)));
            }
            uVar6 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 1)));
          }
          else {
            uVar6 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 1)));
          }
        }
LAB_0022e568:
        iVar18 = iVar18 + 1;
        puVar16 = puVar16 + 0x270;
        iVar19 = iVar19 + 0x270;
      } while (iVar18 < (int)uVar6);
    }
    uVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x28)));
  }
  if ((uVar6 & 0x10000) == 0) {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x22))) != '\x1d') {
      gs_map_draw_functions_race(param_2);
      raceMapMarkers(param_2);
      uVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x28)));
    }
    if ((uVar6 & 0x4000) == 0) {
      raceHudDraw(uVar2);
    }
  }
}


RT_GUEST_FN(0x0022ddd0, guest::raceHudTask)

}  // namespace guest
