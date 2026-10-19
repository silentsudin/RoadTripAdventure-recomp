// guest:0x00229758 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CommitSaveSlotTask(uint32_t param_1, ...) {
  char cVar1;
uint32_t puVar2;
uint32_t puVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  undefined8 uVar6;
  int iVar7;
  
  iVar7 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8))) == 0) {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x1c))) == 0) {
      puVar2 = 0x01782ba8;
      puVar3 = 0x00110000;
      do {
        uVar4 = (*(uint64_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(1) * sizeof(uint64_t)));
        uVar5 = (*(uint64_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(2) * sizeof(uint64_t)));
        uVar6 = (*(uint64_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(3) * sizeof(uint64_t)));
        (*(uint64_t*)rtguest::G((uint32_t)puVar3)) = (*(uint64_t*)rtguest::G((uint32_t)puVar2));
        (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint64_t))) = uVar4;
        (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint64_t))) = uVar5;
        (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(3) * sizeof(uint64_t))) = uVar6;
        puVar2 = puVar2 + 0x20;  // T* element step
        puVar3 = puVar3 + 0x20;  // T* element step
      } while (puVar2 != (uint32_t)0x1785fe8);
      (*(uint64_t*)rtguest::G((uint32_t)puVar3)) = (*(uint8_t*)rtguest::G(0x01785fe8));
    }
    else {
      puVar2 = 0x177f760;
      puVar3 = 0x00110000;
      do {
        uVar4 = (*(uint64_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(1) * sizeof(uint64_t)));
        uVar5 = (*(uint64_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(2) * sizeof(uint64_t)));
        uVar6 = (*(uint64_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(3) * sizeof(uint64_t)));
        (*(uint64_t*)rtguest::G((uint32_t)puVar3)) = (*(uint64_t*)rtguest::G((uint32_t)puVar2));
        (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint64_t))) = uVar4;
        (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint64_t))) = uVar5;
        (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(3) * sizeof(uint64_t))) = uVar6;
        puVar2 = puVar2 + 0x20;  // T* element step
        puVar3 = puVar3 + 0x20;  // T* element step
      } while (puVar2 != (uint32_t)0x1782ba0);
      (*(uint64_t*)rtguest::G((uint32_t)puVar3)) = (*(uint8_t*)rtguest::G(0x01782ba0));
    }
    gs_tasks_spawn(param_1,0x2291c8,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x1c))) | (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x1c))) << 8);
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x24)));
  }
  else {
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x24)));
  }
  if (cVar1 != '\0') {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x1c))) == 0) {
      puVar2 = 0x01782ba8;
      puVar3 = 0x00110000;
      do {
        uVar6 = (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint64_t)));
        uVar4 = (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint64_t)));
        uVar5 = (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(3) * sizeof(uint64_t)));
        (*(uint64_t*)rtguest::G((uint32_t)puVar2)) = (*(uint64_t*)rtguest::G((uint32_t)puVar3));
        (*(uint64_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(1) * sizeof(uint64_t))) = uVar6;
        (*(uint64_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(2) * sizeof(uint64_t))) = uVar4;
        (*(uint64_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(3) * sizeof(uint64_t))) = uVar5;
        puVar3 = puVar3 + 0x20;  // T* element step
        puVar2 = puVar2 + 0x20;  // T* element step
      } while (puVar3 != (uint32_t)0x113440);
      (*(uint64_t*)rtguest::G((uint32_t)puVar2)) = (*(uint8_t*)rtguest::G(0x00113440));
    }
    else {
      puVar2 = 0x177f760;
      puVar3 = 0x00110000;
      do {
        uVar5 = (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint64_t)));
        uVar6 = (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint64_t)));
        uVar4 = (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(3) * sizeof(uint64_t)));
        (*(uint64_t*)rtguest::G((uint32_t)puVar2)) = (*(uint64_t*)rtguest::G((uint32_t)puVar3));
        (*(uint64_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(1) * sizeof(uint64_t))) = uVar5;
        (*(uint64_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(2) * sizeof(uint64_t))) = uVar6;
        (*(uint64_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(3) * sizeof(uint64_t))) = uVar4;
        puVar3 = puVar3 + 0x20;  // T* element step
        puVar2 = puVar2 + 0x20;  // T* element step
      } while (puVar3 != (uint32_t)0x113440);
      (*(uint64_t*)rtguest::G((uint32_t)puVar2)) = (*(uint8_t*)rtguest::G(0x00113440));
    }
    iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 4))) = 1;
    gs_tasks_kill(param_1);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x00229758, guest::CommitSaveSlotTask)

}  // namespace guest
