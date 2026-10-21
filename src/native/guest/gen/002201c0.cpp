// guest:0x002201c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawFieldTerrainTiles(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x28, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x38, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x20, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x48) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4c) = 0x00000000u;
    rtguest::setS32(2, (uint32_t)param_4);  // walk: = param_4
    rtguest::setS32(3, (uint32_t)param_3);  // walk: = param_3
  }
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
        uVar5 = RT_CALLRA(0x22022c, guest::ComputeGridCellIndex(iVar2 + -0x1000000,iVar3 + -0x2000000));
        puVar7 = (uint32_t)(param_4 + (uVar5 & 3) * 4 + 0x60);
        if (((*(uint32_t*)rtguest::G((uint32_t)puVar7)) & 0x7fffffff) != uVar5) {
          (*(uint32_t*)rtguest::G((uint32_t)puVar7)) = uVar5 | 0x80000000;
          RT_CALLRA(0x220278, guest::gs_tasks_spawn(param_3,0x20dc50,uVar5 | 0x390000));
        }
        (*(uint8_t*)rtguest::G((uint32_t)0x3335b8 + (uint32_t)(uVar5 & 3) * 1)) = 5;
      }
    }
    else {
      uVar5 = RT_CALLRA(0x2202ac, guest::ComputeGridCellIndex(iVar2 + -0x1000000,iVar3 + 0x2000000));
      puVar7 = (uint32_t)(param_4 + (uVar5 & 3) * 4 + 0x60);
      if (((*(uint32_t*)rtguest::G((uint32_t)puVar7)) & 0x7fffffff) != uVar5) {
        (*(uint32_t*)rtguest::G((uint32_t)puVar7)) = uVar5 | 0x80000000;
        RT_CALLRA(0x2202f8, guest::gs_tasks_spawn(param_3,0x20dc50,uVar5 | 0x390000));
      }
      (*(uint8_t*)rtguest::G((uint32_t)0x3335b8 + (uint32_t)(uVar5 & 3) * 1)) = 0;
    }
  }
  if (0x400000 < uVar6) {
    if (param_2 < 0x1400001) {
      if (0xbfffff < param_2) goto LAB_00220428;
      uVar5 = RT_CALLRA(0x220344, guest::ComputeGridCellIndex((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x58))) + 0x1000000,(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x5c))) + -0x2000000))
      ;
      uVar8 = uVar5 & 3;
      puVar7 = (uint32_t)(param_4 + uVar8 * 4 + 0x60);
      if (((*(uint32_t*)rtguest::G((uint32_t)puVar7)) & 0x7fffffff) != uVar5) {
        (*(uint32_t*)rtguest::G((uint32_t)puVar7)) = uVar5 | 0x80000000;
        RT_CALLRA(0x220390, guest::gs_tasks_spawn(param_3,0x20dc50,uVar5 | 0x390000));
      }
      uVar4 = 6;
    }
    else {
      uVar5 = RT_CALLRA(0x2203c4, guest::ComputeGridCellIndex((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x58))) + 0x1000000,(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x5c))) + 0x2000000));
      uVar8 = uVar5 & 3;
      puVar7 = (uint32_t)(param_4 + uVar8 * 4 + 0x60);
      if (((*(uint32_t*)rtguest::G((uint32_t)puVar7)) & 0x7fffffff) != uVar5) {
        (*(uint32_t*)rtguest::G((uint32_t)puVar7)) = uVar5 | 0x80000000;
        RT_CALLRA(0x220410, guest::gs_tasks_spawn(param_3,0x20dc50,uVar5 | 0x390000));
      }
      uVar4 = 1;
    }
    (*(uint8_t*)rtguest::G((uint32_t)0x3335b8 + (uint32_t)(uVar8) * 1)) = uVar4;
  }
LAB_00220428:
  if (uVar6 < 0xc00000) {
    uVar6 = RT_CALLRA(0x220444, guest::ComputeGridCellIndex((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x58))) + -0x2000000,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x5c)))));
    uVar5 = uVar6 & 3;
    puVar7 = (uint32_t)(param_4 + uVar5 * 4 + 0x60);
    if (((*(uint32_t*)rtguest::G((uint32_t)puVar7)) & 0x7fffffff) != uVar6) {
      (*(uint32_t*)rtguest::G((uint32_t)puVar7)) = uVar6 | 0x80000000;
      RT_CALLRA(0x220490, guest::gs_tasks_spawn(param_3,0x20dc50,uVar6 | 0x390000));
    }
    uVar4 = 2;
  }
  else {
    if (uVar6 < 0x1400001) {
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0xb)));
      goto LAB_0022051c;
    }
    uVar6 = RT_CALLRA(0x2204b8, guest::ComputeGridCellIndex((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x58))) + 0x2000000,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x5c)))));
    uVar5 = uVar6 & 3;
    puVar7 = (uint32_t)(param_4 + uVar5 * 4 + 0x60);
    if (((*(uint32_t*)rtguest::G((uint32_t)puVar7)) & 0x7fffffff) != uVar6) {
      (*(uint32_t*)rtguest::G((uint32_t)puVar7)) = uVar6 | 0x80000000;
      RT_CALLRA(0x220504, guest::gs_tasks_spawn(param_3,0x20dc50,uVar6 | 0x390000));
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
    RT_CALLRA(0x220584, guest::uiDebugText8(400,0x20,auStack_50,0xf));
    uVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 100)));
    if ((int)uVar6 < 0) {
      ::sprintf((char *)rtguest::G((uint32_t)(auStack_50)), (char *)rtguest::G((uint32_t)(0x3335c0)));
    }
    else {
      ::sprintf((char *)rtguest::G((uint32_t)(auStack_50)), (char *)rtguest::G((uint32_t)(0x2eaed8)), (int)uVar6 >> 4, (int)uVar6 >> 2 & 3, (int)(uVar6 & 2) >> 1 | (uVar6 & 1) << 1);
    }
    RT_CALLRA(0x2205ec, guest::uiDebugText8(0x1b8,0x20,auStack_50,0xf));
    uVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x68)));
    if ((int)uVar6 < 0) {
      ::sprintf((char *)rtguest::G((uint32_t)(auStack_50)), (char *)rtguest::G((uint32_t)(0x3335c0)));
    }
    else {
      ::sprintf((char *)rtguest::G((uint32_t)(auStack_50)), (char *)rtguest::G((uint32_t)(0x2eaed8)), (int)uVar6 >> 4, (int)uVar6 >> 2 & 3, (int)(uVar6 & 2) >> 1 | (uVar6 & 1) << 1);
    }
    RT_CALLRA(0x220654, guest::uiDebugText8(0x1e0,0x20,auStack_50,0xf));
    uVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x6c)));
    if ((int)uVar6 < 0) {
      ::sprintf((char *)rtguest::G((uint32_t)(auStack_50)), (char *)rtguest::G((uint32_t)(0x3335c0)));
    }
    else {
      ::sprintf((char *)rtguest::G((uint32_t)(auStack_50)), (char *)rtguest::G((uint32_t)(0x2eaed8)), (int)uVar6 >> 4, (int)uVar6 >> 2 & 3, (int)(uVar6 & 2) >> 1 | (uVar6 & 1) << 1);
    }
    RT_CALLRA(0x2206c4, guest::uiDebugText8(0x208,0x20,auStack_50,0xf));
    ::sprintf((char *)rtguest::G((uint32_t)(auStack_50)), (char *)rtguest::G((uint32_t)(0x3335c8)), (*(uint8_t*)rtguest::G(0x3335b8)));
    RT_CALLRA(0x2206ec, guest::uiDebugText8(0x1a0,0x28,auStack_50,0xf));
    ::sprintf((char *)rtguest::G((uint32_t)(auStack_50)), (char *)rtguest::G((uint32_t)(0x3335c8)), (*(uint32_t*)rtguest::G(0x3335b9)));
    RT_CALLRA(0x220710, guest::uiDebugText8(0x1c8,0x28,auStack_50,0xf));
    ::sprintf((char *)rtguest::G((uint32_t)(auStack_50)), (char *)rtguest::G((uint32_t)(0x3335c8)), (*(uint32_t*)rtguest::G(0x3335ba)));
    RT_CALLRA(0x220734, guest::uiDebugText8(0x1f0,0x28,auStack_50,0xf));
    ::sprintf((char *)rtguest::G((uint32_t)(auStack_50)), (char *)rtguest::G((uint32_t)(0x3335c8)), (*(uint32_t*)rtguest::G(0x3335bb)));
    RT_CALLRA(0x220760, guest::uiDebugText8(0x218,0x28,auStack_50,0xf));
    ::sprintf((char *)rtguest::G((uint32_t)(auStack_50)), (char *)rtguest::G((uint32_t)(0x3335d0)), (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x58))));
    RT_CALLRA(0x220784, guest::uiDebugText8(0x1a0,0x18,auStack_50,0xf));
    ::sprintf((char *)rtguest::G((uint32_t)(auStack_50)), (char *)rtguest::G((uint32_t)(0x3335d0)), (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x5c))));
    RT_CALLRA(0x2207a8, guest::uiDebugText8(0x1f0,0x18,auStack_50,0xf));
  }
  return 0;
}


RT_GUEST_FN(0x002201c0, guest::DrawFieldTerrainTiles)

}  // namespace guest
