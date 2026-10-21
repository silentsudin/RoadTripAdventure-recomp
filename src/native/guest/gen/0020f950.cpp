// guest:0x0020f950 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t StartRaceModeTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x48) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4c) = 0x00000000u;
  }
uint32_t pcVar1;
  char cVar2;
  undefined1 uVar3;
  char cVar4;
  undefined4 uVar5;
  int iVar6;
  uint uVar7;
uint32_t pcVar8;
uint32_t piVar9;
  uint uVar10;
  undefined8 uVar11;
  byte bVar12;
  int iVar13;
uint32_t puVar14;
  int iVar15;
uint32_t puVar16;
  int iVar17;
  uint uVar18;
  
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0x800 | 0x410000;
  RT_CALLRA(0x20f998, guest::SelectMeshIntersectionRoutine(1));
  RT_CALLRA(0x20f9a0, guest::SelectScene3dHandler(1));
  iVar17 = (int)param_1;
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10))) == 1) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x10))) =
         0x2be158 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x1c))) * 0x10;
  }
  else if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10))) & 2) == 0) {
    iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x1c)));
    if (iVar6 < 0x23) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x10))) = 0x2bddd0 + iVar6 * 0x10;
    }
    else {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x10))) = 0x2bdde8u + iVar6 * 4;
    }
  }
  else {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x10))) =
         0x2be1d8 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x1c))) * 0x10;
  }
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x20))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x1c)));
  uVar3 = (*(uint8_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x10)))));
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x22))) = uVar3;
  RT_CALLRA(0x20fa58, guest::UpdateSceneLightsAndFog(0,(int8_t)uVar3));  // MIPS 0x20fa50/5c sll+sra sign-extends the lbu byte
  uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x10))) + 0xc)));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x14))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002a88e8 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x22))) * 8)));
  RT_CALLRA(0x20fa7c, guest::SetAICarController(uVar5));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x60))) = 0xffffffff;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x54))) = 0x40;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x6c))) = 0xffffffff;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x68))) = 0xffffffff;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 100))) = 0xffffffff;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x5c))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x58))) = 0;
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10))) == '\b') {
    bVar12 = 8;
  }
  else {
    cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x22)));
    cVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x22)));
    if (cVar2 == '\x12') {
      uVar11 = 0x40029;
    }
    else if (cVar2 == '\x13') {
      uVar11 = 0x40007;
    }
    else if (cVar2 == '\x14') {
      uVar11 = 0x4001a;
    }
    else if (cVar2 == '\x15') {
      uVar11 = 0x4001b;
    }
    else if (cVar2 == '\x16') {
      uVar11 = 0x4001c;
    }
    else if (cVar2 == '\x17') {
      uVar11 = 0x40012;
    }
    else if (cVar2 == '\x18') {
      uVar11 = 0x4001d;
    }
    else if (cVar2 == '\x19') {
      uVar11 = 0x40010;
    }
    else if (cVar2 == '\x1a') {
      uVar11 = 0x40011;
    }
    else if (cVar2 == '\x1b') {
      uVar11 = 0x4001e;
    }
    else if (cVar4 == '\x1c') {
      uVar11 = 0x4001f;
    }
    else if (cVar4 == '\x1d') {
      uVar11 = 0x40020;
    }
    else if (cVar4 == '\x1e') {
      uVar11 = 0x40021;
    }
    else if (cVar4 == '\x1f') {
      uVar11 = 0x40022;
    }
    else if (cVar4 == ' ') {
      uVar11 = 0x40023;
    }
    else if (cVar4 == '!') {
      uVar11 = 0x40024;
    }
    else if (cVar4 == '#') {
      uVar11 = 0x40025;
    }
    else if (cVar4 == '$') {
      uVar11 = 0x40026;
    }
    else if (cVar4 == '%') {
      uVar11 = 0x40027;
    }
    else if (cVar4 == '&') {
      uVar11 = 0x40028;
    }
    else {
      uVar11 = 0x4000d;
    }
    RT_CALLRA(0x20fd60, guest::gs_tasks_spawn(param_1,0x20dc50,uVar11));
    bVar12 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10)));
  }
  if ((bVar12 & 2) == 0) {
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0x800) == 0) {
      iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x10)));
      if (bVar12 == 8) {
        if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) != (uint32_t)0x0) && (1 < (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 1))))) {
          uVar7 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 1))) - 1;
          iVar13 = 0;
          if (0 < (int)uVar7) {
            pcVar8 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4)))));
            piVar9 = 0x0177ea5c;
            do {
              cVar2 = (*(int8_t*)rtguest::G((uint32_t)pcVar8));
              iVar13 = iVar13 + 1;
              pcVar1 = pcVar8 + 1;
              pcVar8 = pcVar8 + 2;
              (*(int32_t*)rtguest::G((uint32_t)piVar9)) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x2c22c8 + cVar2 * 4))) + (*(int8_t*)rtguest::G((uint32_t)pcVar1)) * 0x10;
              piVar9 = piVar9 + 0x4;  // T* element step
            } while (iVar13 < (int)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 1))) - 1));
            uVar7 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 1))) - 1;
          }
          RT_CALLRA(0x20fd84, guest::gs_tasks_spawn(param_1,0x20dc50,uVar7 | 0x370300));
        }
        uVar7 = 0;
        RT_CALLRA(0x20fde0, guest::gs_tasks_spawn(param_1,0x20dc50,0x750000));
        RT_CALLRA(0x20fd90, guest::SpawnGameViewTasks(param_1,0));
        iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x10)));
        iVar13 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 1))) - 2;
        if (0 < iVar13) {
          uVar18 = iVar13 * 0x20 + 0x60;
          puVar16 = 0x01787008;
          piVar9 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4)));
          iVar15 = iVar13;
          while( true ) {
            uVar10 = iVar15 << 10 | uVar18;
            uVar18 = uVar18 - 0x20;
            uVar10 = uVar10 | uVar7;
            uVar7 = uVar7 + 1;
            puVar14 = (uint32_t)((*(int32_t*)rtguest::G((uint32_t)piVar9)) + iVar13 * 2);
            uVar11 = RT_CALLRA(0x20fec0, guest::gs_tasks_spawn(param_1,0x218458,uVar10 | 0x800000));
            iVar13 = iVar13 + -1;
            RT_CALLRA(0x20ff4c, guest::SetTaskFlag(uVar11));
            (*(uint8_t*)rtguest::G((uint32_t)puVar16 + (uint32_t)(3) * sizeof(uint8_t))) = 0x80;
            (*(uint8_t*)rtguest::G((uint32_t)puVar16)) = (*(uint8_t*)rtguest::G((uint32_t)puVar14));
            uVar3 = (*(uint8_t*)rtguest::G((uint32_t)puVar14 + (uint32_t)(1) * sizeof(uint8_t)));
            (*(uint8_t*)rtguest::G((uint32_t)puVar16 + (uint32_t)(2) * sizeof(uint8_t))) = (char)iVar15;
            iVar15 = iVar15 + -1;
            (*(uint8_t*)rtguest::G((uint32_t)puVar16 + (uint32_t)(1) * sizeof(uint8_t))) = uVar3;
            puVar16 = puVar16 + 4;
            if ((uVar7 == (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 1)))) || (iVar13 < 1)) break;
            piVar9 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4)));
          }
        }
      }
      else {
        if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) != (uint32_t)0x0) && (1 < (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 1))))) {
          bVar12 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 1)));
          iVar13 = 0;
          if (0 < (int)(bVar12 - 1)) {
            pcVar8 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4)))));
            piVar9 = 0x0177ea5c;
            do {
              cVar2 = (*(int8_t*)rtguest::G((uint32_t)pcVar8));
              iVar13 = iVar13 + 1;
              pcVar1 = pcVar8 + 1;
              pcVar8 = pcVar8 + 2;
              (*(int32_t*)rtguest::G((uint32_t)piVar9)) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x2c22c8 + cVar2 * 4))) + (*(int8_t*)rtguest::G((uint32_t)pcVar1)) * 0x10;
              bVar12 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 1)));
              piVar9 = piVar9 + 0x4;  // T* element step
            } while (iVar13 < (int)(bVar12 - 1));
          }
          RT_CALLRA(0x20fed4, guest::gs_tasks_spawn(param_1,0x20dc50,bVar12 - 1 | 0x70300));
        }
        RT_CALLRA(0x20ff40, guest::gs_tasks_spawn(param_1,0x20dc50,0x450000));
        RT_CALLRA(0x20fdec, guest::SpawnGameViewTasks(param_1,0));
        RT_CALLRA(0x20fdbc, guest::SpawnRaceRivalTasks(param_1,(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x10))),
                            2 < (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x10))) + 1))),0));
      }
    }
    else {
      RT_CALLRA(0x210028, guest::gs_tasks_spawn(param_1,0x20dc50,0x450000));
      RT_CALLRA(0x20fee0, guest::SpawnGameViewTasks(param_1,0));
      RT_CALLRA(0x20fe00, guest::SpawnRaceRivalTasks(param_1,0,0,0));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 0x104;
    }
  }
  else {
    RT_CALLRA(0x210038, guest::gs_tasks_spawn(param_1,0x20dc50,0x450000));
    RT_CALLRA(0x210044, guest::SpawnGameViewTasks(param_1,1));
    RT_CALLRA(0x210060, guest::SpawnRaceRivalTasks(param_1,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x10))),
                        (*(int8_t*)rtguest::G(0x177fdd6)) != '\0' && (*(uint32_t*)rtguest::G(0x178321e)) != '\0',1));
  }
  uVar11 = RT_CALLRA(0x210074, guest::gs_tasks_spawn(param_1,0x2179d8,0));
  RT_CALLRA(0x210080, guest::SetTaskFlag(uVar11));
  bVar12 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10)));
  if (bVar12 == 8) {
LAB_002100f0:
    uVar7 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28)));
  }
  else {
    uVar11 = RT_CALLRA(0x2100a4, guest::gs_tasks_spawn(param_1,0x2490a0,bVar12 >> 1 & 1));
    RT_CALLRA(0x2100ac, guest::SetTaskFlag(uVar11));
    bVar12 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10)));
    if (bVar12 != 8) {
      iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x10))) + 8)));
      if (iVar6 != 0) {
        uVar11 = RT_CALLRA(0x2100d8, guest::gs_tasks_spawn(param_1,iVar6,bVar12 >> 1 & 1));
        RT_CALLRA(0x2100e0, guest::SetTaskFlag(uVar11));
        bVar12 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10)));
        goto LAB_002100fc;
      }
      goto LAB_002100f0;
    }
    uVar7 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28)));
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = uVar7 | 0x104;
LAB_002100fc:
  if (bVar12 != 8) {
    RT_CALLRA(0x210110, guest::gs_tasks_spawn(param_1,0x22c4a8,0));
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0xc))) = 0x20f3b8;
  return 0;
}


RT_GUEST_FN(0x0020f950, guest::StartRaceModeTask)

}  // namespace guest
