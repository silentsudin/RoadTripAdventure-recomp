// guest:0x0025b500 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t initRaceStandings(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(0);
  undefined4 uVar1;
uint32_t puVar2;
uint32_t puVar3;
uint32_t puVar4;
uint32_t puVar5;
  int iVar6;
uint32_t puVar7;
uint32_t puVar8;
  
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x80a8))) = param_2;
  puVar3 = (uint32_t)(param_1 + 0x8110);
  puVar5 = (uint32_t)(param_1 + 0x80d4);
  puVar4 = (uint32_t)(param_1 + 0x80fc);
  puVar8 = 0x01780a14;
  puVar7 = 0x002c3af8;
  iVar6 = 0;
  puVar2 = (uint32_t)(param_1 + 33000);
  do {
    (*(uint32_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(-0x14) * sizeof(uint32_t))) = 0x7fffffff;
    iVar6 = iVar6 + 1;
    (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(-0x14) * sizeof(uint32_t))) = 0x7fffffff;
    (*(uint32_t*)rtguest::G((uint32_t)puVar5)) = 0x7fffffff;
    puVar5 = puVar5 + (uint32_t)(1) * sizeof(uint32_t);
    (*(uint32_t*)rtguest::G((uint32_t)puVar2)) = 0;
    puVar2 = puVar2 + (uint32_t)(1) * sizeof(uint32_t);
    uVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar7));
    puVar7 = puVar7 + (uint32_t)(1) * sizeof(uint32_t);
    (*(uint32_t*)rtguest::G((uint32_t)puVar3)) = uVar1;
    puVar3 = puVar3 + (uint32_t)(1) * sizeof(uint32_t);
    uVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar8));
    puVar8 = puVar8 + (uint32_t)(1) * sizeof(uint32_t);
    (*(uint32_t*)rtguest::G((uint32_t)puVar4)) = uVar1;
    puVar4 = puVar4 + (uint32_t)(1) * sizeof(uint32_t);
  } while (iVar6 < 5);
  return 0;
}


RT_GUEST_FN(0x0025b500, guest::initRaceStandings)

}  // namespace guest
