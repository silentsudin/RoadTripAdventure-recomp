// guest:0x0021ccb8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RestoreControlStateFromReplay(uint32_t param_1, ...) {
uint32_t puVar1;
uint32_t puVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  
  rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2eae90u)), (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x44))) + -0x100000);
  rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2eaea8u)), (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x48))) + -0x100000);
  rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2eaec0u)), (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x4c))) + -0x100000);
  puVar1 = 0x177ac50;
  puVar2 = (uint32_t)0x00100000;
  do {
    uVar3 = (*(uint64_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(1) * sizeof(uint64_t)));
    uVar4 = (*(uint64_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(2) * sizeof(uint64_t)));
    uVar5 = (*(uint64_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(3) * sizeof(uint64_t)));
    (*(uint64_t*)rtguest::G((uint32_t)puVar1)) = (*(uint64_t*)rtguest::G((uint32_t)puVar2));
    (*(uint64_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(1) * sizeof(uint64_t))) = uVar3;
    (*(uint64_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(2) * sizeof(uint64_t))) = uVar4;
    (*(uint64_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(3) * sizeof(uint64_t))) = uVar5;
    puVar2 = puVar2 + 0x20;  // T* element step
    puVar1 = puVar1 + 0x20;  // T* element step
  } while (puVar2 != (uint32_t)0x00103a80);
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x14))) = (*(uint32_t*)rtguest::G(0x103a80));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x4c))) = 0x00103a8c;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x44))) = 0x00103a84;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x48))) = 0x00103a88;
  return 0;
}


RT_GUEST_FN(0x0021ccb8, guest::RestoreControlStateFromReplay)

}  // namespace guest
