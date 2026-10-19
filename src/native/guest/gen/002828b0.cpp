// guest:0x002828b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __sfp(uint32_t param_1, ...) {
  int iVar1;
uint32_t puVar2;
uint32_t puVar3;
uint32_t puVar4;
  
  puVar4 = (uint32_t)param_1;
  puVar2 = puVar4 + 0x76;
  if ((*(uint32_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(0xe) * sizeof(uint32_t))) == 0) {
    __sinit();
  }
  do {
    do {
      puVar3 = puVar2;
      iVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint32_t)));
      puVar2 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint32_t)));
      while (iVar1 = iVar1 + -1, -1 < iVar1) {
        if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(puVar2 + 3))) == 0) {
          (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar2 + 0xe))) = 0xffff;
          (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(0x15) * sizeof(uint32_t))) = puVar4;
          (*(uint32_t*)rtguest::G((uint32_t)puVar2)) = 0;
          (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(2) * sizeof(uint32_t))) = 0;
          (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(1) * sizeof(uint32_t))) = 0;
          (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(4) * sizeof(uint32_t))) = 0;
          (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(5) * sizeof(uint32_t))) = 0;
          (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(6) * sizeof(uint32_t))) = 0;
          (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(0xc) * sizeof(uint32_t))) = 0;
          (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(0xd) * sizeof(uint32_t))) = 0;
          (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(0x11) * sizeof(uint32_t))) = 0;
          (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(0x12) * sizeof(uint32_t))) = 0;
          (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(puVar2 + 3))) = 1;
          return puVar2;
        }
        puVar2 = puVar2 + 0x16;
      }
      puVar2 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar3));
    } while ((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar3)) != (uint32_t)0x0);
    puVar2 = (uint32_t)__sfmoreglue(param_1,4);
    (*(uint32_t*)rtguest::G((uint32_t)puVar3)) = puVar2;
  } while (puVar2 != (uint32_t)0x0);
  (*(uint32_t*)rtguest::G((uint32_t)puVar4)) = 0xc;
  return (uint32_t)0x0;
}


RT_GUEST_FN(0x002828b0, guest::__sfp)

}  // namespace guest
