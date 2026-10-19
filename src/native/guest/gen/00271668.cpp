// guest:0x00271668 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t textGridReadCellAdvance(uint32_t param_1, ...) {
  uint uVar1;
  undefined8 uVar2;
uint32_t puVar3;
  
  puVar3 = (uint32_t)param_1;
  uVar2 = textBufferGetPixel16(param_1,(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(4) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(5) * sizeof(uint32_t))));
  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(4) * sizeof(uint32_t)));
  (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(4) * sizeof(uint32_t))) = uVar1 + 1;
  if ((*(uint32_t*)rtguest::G((uint32_t)puVar3)) <= uVar1 + 1) {
    if ((*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(5) * sizeof(uint32_t))) < (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint32_t)))) {
      (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(4) * sizeof(uint32_t))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(5) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(5) * sizeof(uint32_t))) + 1;
    }
    else {
      (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(5) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint32_t)));
      (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(4) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)puVar3));
    }
  }
}


RT_GUEST_FN(0x00271668, guest::textGridReadCellAdvance)

}  // namespace guest
