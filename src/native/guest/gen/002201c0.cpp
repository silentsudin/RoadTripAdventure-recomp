// guest:0x002201c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawFieldTerrainTiles(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  char cVar1;
  int iVar2;
  int iVar3;
  undefined1 uVar4;
  uint uVar5;
  uint uVar6;
uint32_t puVar7;
  uint uVar8;
uint32_t auStack_50 = rtguest::allocFrame(32);
  
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x58)));
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x5c)));
  uVar6 = param_1 - iVar2 & 0x1ffffff;
  param_2 = param_2 - iVar3;
  if (uVar6 < 0x1c00000) {
    if (param_2 < 0x1400001) {
      if (param_2 < 0xc00000) {
        uVar5 = ComputeGridCellIndex(iVar2 + -0x1000000,iVar3 + -0x2000000);
        puVar7 = (uint32_t)(param_4 + (uVar5 & 3) * 4 + 0x60);
        if (((*(uint32_t*)rtguest::G((uint32_t)puVar7)) & 0x7fffffff) != uVar5) {
          (*(uint32_t*)rtguest::G((uint32_t)puVar7)) = uVar5 | 0x80000000;
          gs_tasks_spawn(param_3,0x20dc50,uVar5 | 0x390000);
        }
        (*(uint8_t*)rtguest::G((uint32_t)0x3335b8 + (uint32_t)(uVar5 & 3) * 1)) = 5;
      }
    }
    else {
      uVar5 = ComputeGridCellIndex(iVar2 + -0x1000000,iVar3 + 0x2000000);
      puVar7 = (uint32_t)(param_4 + (uVar5 & 3) * 4 + 0x60);
      if (((*(uint32_t*)rtguest::G((uint32_t)puVar7)) & 0x7fffffff) != uVar5) {
        (*(uint32_t*)rtguest::G((uint32_t)puVar7)) = uVar5 | 0x80000000;
        gs_tasks_spawn(param_3,0x20dc50,uVar5 | 0x390000);
      }
      (*(uint8_t*)rtguest::G((uint32_t)0x3335b8 + (uint32_t)(uVar5 & 3) * 1)) = 0;
    }
  }
  if (0x400000 < uVar6) {
    if (param_2 < 0x1400001) {
      if (0xbfffff < param_2) goto LAB_00220428;
      uVar5 = ComputeGridCellIndex
                        ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x58))) + 0x1000000,(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x5c))) + -0x2000000)
      ;
      uVar8 = uVar5 & 3;
      puVar7 = (uint32_t)(param_4 + uVar8 * 4 + 0x60);
      if (((*(uint32_t*)rtguest::G((uint32_t)puVar7)) & 0x7fffffff) != uVar5) {
        (*(uint32_t*)rtguest::G((uint32_t)puVar7)) = uVar5 | 0x80000000;
        gs_tasks_spawn(param_3,0x20dc50,uVar5 | 0x390000);
      }
      uVar4 = 6;
    }
    else {
      uVar5 = ComputeGridCellIndex
                        ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x58))) + 0x1000000,(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x5c))) + 0x2000000);
      uVar8 = uVar5 & 3;
      puVar7 = (uint32_t)(param_4 + uVar8 * 4 + 0x60);
      if (((*(uint32_t*)rtguest::G((uint32_t)puVar7)) & 0x7fffffff) != uVar5) {
        (*(uint32_t*)rtguest::G((uint32_t)puVar7)) = uVar5 | 0x80000000;
        gs_tasks_spawn(param_3,0x20dc50,uVar5 | 0x390000);
      }
      uVar4 = 1;
    }
    (*(uint8_t*)rtguest::G((uint32_t)0x3335b8 + (uint32_t)(uVar8) * 1)) = uVar4;
  }
LAB_00220428:
  if (uVar6 < 0xc00000) {
    uVar6 = ComputeGridCellIndex
                      ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x58))) + -0x2000000,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x5c))));
    uVar5 = uVar6 & 3;
    puVar7 = (uint32_t)(param_4 + uVar5 * 4 + 0x60);
    if (((*(uint32_t*)rtguest::G((uint32_t)puVar7)) & 0x7fffffff) != uVar6) {
      (*(uint32_t*)rtguest::G((uint32_t)puVar7)) = uVar6 | 0x80000000;
      gs_tasks_spawn(param_3,0x20dc50,uVar6 | 0x390000);
    }
    uVar4 = 2;
  }
  else {
    if (uVar6 < 0x1400001) {
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0xb)));
      goto LAB_0022051c;
    }
    uVar6 = ComputeGridCellIndex
                      ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x58))) + 0x2000000,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x5c))));
    uVar5 = uVar6 & 3;
    puVar7 = (uint32_t)(param_4 + uVar5 * 4 + 0x60);
    if (((*(uint32_t*)rtguest::G((uint32_t)puVar7)) & 0x7fffffff) != uVar6) {
      (*(uint32_t*)rtguest::G((uint32_t)puVar7)) = uVar6 | 0x80000000;
      gs_tasks_spawn(param_3,0x20dc50,uVar6 | 0x390000);
    }
    uVar4 = 4;
  }
  (*(uint8_t*)rtguest::G((uint32_t)0x3335b8 + (uint32_t)(uVar5) * 1)) = uVar4;
  cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0xb)));
LAB_0022051c:
  if (cVar1 != '\0') {
    uVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x60)));
    if ((int)uVar6 < 0) {
      ::sprintf((char *)rtguest::G((uint32_t)(auStack_50)), (char *)rtguest::G((uint32_t)(0x3335c0)));
    }
    else {
      ::sprintf((char *)rtguest::G((uint32_t)(auStack_50)), (char *)rtguest::G((uint32_t)(0x2eaed8)), (int)uVar6 >> 4, (int)uVar6 >> 2 & 3, (int)(uVar6 & 2) >> 1 | (uVar6 & 1) << 1);
    }
    uiDebugText8(400,0x20,auStack_50,0xf);
    uVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 100)));
    if ((int)uVar6 < 0) {
      ::sprintf((char *)rtguest::G((uint32_t)(auStack_50)), (char *)rtguest::G((uint32_t)(0x3335c0)));
    }
    else {
      ::sprintf((char *)rtguest::G((uint32_t)(auStack_50)), (char *)rtguest::G((uint32_t)(0x2eaed8)), (int)uVar6 >> 4, (int)uVar6 >> 2 & 3, (int)(uVar6 & 2) >> 1 | (uVar6 & 1) << 1);
    }
    uiDebugText8(0x1b8,0x20,auStack_50,0xf);
    uVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x68)));
    if ((int)uVar6 < 0) {
      ::sprintf((char *)rtguest::G((uint32_t)(auStack_50)), (char *)rtguest::G((uint32_t)(0x3335c0)));
    }
    else {
      ::sprintf((char *)rtguest::G((uint32_t)(auStack_50)), (char *)rtguest::G((uint32_t)(0x2eaed8)), (int)uVar6 >> 4, (int)uVar6 >> 2 & 3, (int)(uVar6 & 2) >> 1 | (uVar6 & 1) << 1);
    }
    uiDebugText8(0x1e0,0x20,auStack_50,0xf);
    uVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x6c)));
    if ((int)uVar6 < 0) {
      ::sprintf((char *)rtguest::G((uint32_t)(auStack_50)), (char *)rtguest::G((uint32_t)(0x3335c0)));
    }
    else {
      ::sprintf((char *)rtguest::G((uint32_t)(auStack_50)), (char *)rtguest::G((uint32_t)(0x2eaed8)), (int)uVar6 >> 4, (int)uVar6 >> 2 & 3, (int)(uVar6 & 2) >> 1 | (uVar6 & 1) << 1);
    }
    uiDebugText8(0x208,0x20,auStack_50,0xf);
    ::sprintf((char *)rtguest::G((uint32_t)(auStack_50)), (char *)rtguest::G((uint32_t)(0x3335c8)), (*(uint8_t*)rtguest::G(0x3335b8)));
    uiDebugText8(0x1a0,0x28,auStack_50,0xf);
    ::sprintf((char *)rtguest::G((uint32_t)(auStack_50)), (char *)rtguest::G((uint32_t)(0x3335c8)), (*(uint32_t*)rtguest::G(0x3335b9)));
    uiDebugText8(0x1c8,0x28,auStack_50,0xf);
    ::sprintf((char *)rtguest::G((uint32_t)(auStack_50)), (char *)rtguest::G((uint32_t)(0x3335c8)), (*(uint32_t*)rtguest::G(0x3335ba)));
    uiDebugText8(0x1f0,0x28,auStack_50,0xf);
    ::sprintf((char *)rtguest::G((uint32_t)(auStack_50)), (char *)rtguest::G((uint32_t)(0x3335c8)), (*(uint32_t*)rtguest::G(0x3335bb)));
    uiDebugText8(0x218,0x28,auStack_50,0xf);
    ::sprintf((char *)rtguest::G((uint32_t)(auStack_50)), (char *)rtguest::G((uint32_t)(0x3335d0)), (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x58))));
    uiDebugText8(0x1a0,0x18,auStack_50,0xf);
    ::sprintf((char *)rtguest::G((uint32_t)(auStack_50)), (char *)rtguest::G((uint32_t)(0x3335d0)), (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x5c))));
    uiDebugText8(0x1f0,0x18,auStack_50,0xf);
  }
  return 0;
}


RT_GUEST_FN(0x002201c0, guest::DrawFieldTerrainTiles)

}  // namespace guest
