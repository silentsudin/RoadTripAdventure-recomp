// guest:0x00250dd0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CopyAiRouteTables(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(0);
  int iVar1;
uint32_t puVar2;
uint32_t puVar3;
  int iVar4;
  undefined8 uVar5;
  undefined8 uVar6;
  undefined8 uVar7;
  
  iVar4 = 0;
  do {
    iVar1 = (char)(*(int8_t*)rtguest::G((uint32_t)0x2ac098 + (uint32_t)iVar4)) * 0x100;
    puVar2 = (uint32_t)(0x002a9b80 + iVar1);
    if (((uint32_t)param_1 & 7) == 0) {
      puVar3 = param_1;
      do {
        uVar5 = (*(uint64_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(1) * sizeof(uint64_t)));
        uVar6 = (*(uint64_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(2) * sizeof(uint64_t)));
        uVar7 = (*(uint64_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(3) * sizeof(uint64_t)));
        (*(uint64_t*)rtguest::G((uint32_t)puVar3)) = (*(uint64_t*)rtguest::G((uint32_t)puVar2));
        (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint64_t))) = uVar5;
        (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint64_t))) = uVar6;
        (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(3) * sizeof(uint64_t))) = uVar7;
        puVar2 = puVar2 + 4 * sizeof(uint64_t);
        puVar3 = puVar3 + 4 * sizeof(uint64_t);
      } while (puVar2 != (uint32_t)(iVar1 + 0x2a9c80));
    }
    else {
      puVar3 = param_1;
      do {
        uVar5 = (*(uint64_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(1) * sizeof(uint64_t)));
        uVar6 = (*(uint64_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(2) * sizeof(uint64_t)));
        uVar7 = (*(uint64_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(3) * sizeof(uint64_t)));
        (*(uint64_t*)rtguest::G((uint32_t)puVar3)) = (*(uint64_t*)rtguest::G((uint32_t)puVar2));
        (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint64_t))) = uVar5;
        (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint64_t))) = uVar6;
        (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(3) * sizeof(uint64_t))) = uVar7;
        puVar2 = puVar2 + 4 * sizeof(uint64_t);
        puVar3 = puVar3 + 4 * sizeof(uint64_t);
      } while (puVar2 != (uint32_t)(iVar1 + 0x2a9c80));
    }
    iVar4 = iVar4 + 1;
    param_1 = param_1 + 0x20 * sizeof(uint64_t);
  } while (iVar4 < 0x10);
  return 0;
}


RT_GUEST_FN(0x00250dd0, guest::CopyAiRouteTables)

}  // namespace guest
