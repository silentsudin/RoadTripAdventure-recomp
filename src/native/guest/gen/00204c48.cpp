// guest:0x00204c48 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gs_tasks_spawn(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  int iVar1;
  int iVar2;
uint32_t pcVar3;
  
  pcVar3 = 0x177a0d0;
  iVar1 = 0;
  while ((*(int8_t*)rtguest::G((uint32_t)pcVar3)) != '\0') {
    iVar1 = iVar1 + 1;
    pcVar3 = pcVar3 + 0x30;
    if (0x2f < iVar1) {
      fatal(0x2ea048);
      return (uint32_t)0x0;
    }
  }
  (*(int8_t*)rtguest::G((uint32_t)pcVar3)) = '\x01';
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(pcVar3 + 0xc))) = param_2;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(pcVar3 + 0x1c))) = param_3;
  (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(8) * sizeof(int8_t))) = '\0';
  (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(9) * sizeof(int8_t))) = '\0';
  (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(10) * sizeof(int8_t))) = '\0';
  (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(0xb) * sizeof(int8_t))) = '\0';
  (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(0x20) * sizeof(int8_t))) = '\0';
  (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(0x21) * sizeof(int8_t))) = '\0';
  (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(0x22) * sizeof(int8_t))) = '\0';
  (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(0x23) * sizeof(int8_t))) = '\0';
  (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(4) * sizeof(int8_t))) = '\0';
  (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(5) * sizeof(int8_t))) = '\0';
  (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(6) * sizeof(int8_t))) = '\0';
  (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(7) * sizeof(int8_t))) = '\0';
  (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(0x24) * sizeof(int8_t))) = '\0';
  (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(0x10) * sizeof(int8_t))) = '\0';
  (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(0x11) * sizeof(int8_t))) = '\0';
  (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(0x12) * sizeof(int8_t))) = '\0';
  (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(0x13) * sizeof(int8_t))) = '\0';
  (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(0x14) * sizeof(int8_t))) = '\0';
  (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(0x15) * sizeof(int8_t))) = '\0';
  (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(0x16) * sizeof(int8_t))) = '\0';
  (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(0x17) * sizeof(int8_t))) = '\0';
  (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(0x2c) * sizeof(int8_t))) = '\0';
  (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(0x2d) * sizeof(int8_t))) = '\0';
  (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(0x2e) * sizeof(int8_t))) = '\0';
  (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(0x2f) * sizeof(int8_t))) = '\0';
  (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(0x25) * sizeof(int8_t))) = '\0';
  (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(2) * sizeof(int8_t))) = '\0';
  (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(3) * sizeof(int8_t))) = '\0';
  (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(0x26) * sizeof(int8_t))) = '\0';
  (*(int8_t*)rtguest::G((uint32_t)pcVar3 + (uint32_t)(0x27) * sizeof(int8_t))) = '\0';
  iVar1 = (int)param_1;
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(pcVar3 + 0x18))) = iVar1;
  if (param_1 == 0) {
    return pcVar3;
  }
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x10)));
  if (iVar2 != 0) {
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x14)));
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x14))) == 0) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x14))) = pcVar3;
      return pcVar3;
    }
    do {
      iVar2 = iVar1;
      iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x14)));
    } while (iVar1 != 0);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x14))) = pcVar3;
    return pcVar3;
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x10))) = pcVar3;
  return pcVar3;
}


RT_GUEST_FN(0x00204c48, guest::gs_tasks_spawn)

}  // namespace guest
