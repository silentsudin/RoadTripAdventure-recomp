// guest:0x0023db00 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#define GetSlotDataPtr GetSlotDataPtr_decl
#include "guest_decls.inc"
#undef GetSlotDataPtr

namespace guest {

uint32_t GetSlotDataPtr(uint64_t param_1, uint32_t param_2, ...) {
  char cVar1;
uint32_t puVar2;
  int iVar3;
  
  if (param_1 == 0) {
    puVar2 = 0x0177fd8c + param_2 * 0xd12;
  }
  else {
    puVar2 = (uint32_t)0x0;
    iVar3 = (param_2 * 0x1a24 + (int)param_1) * 2;
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0177fdd0 + iVar3 + 2)));
    if (cVar1 != '\0') {
      return (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0177fdd0 + iVar3 + 3))) * 0x10 +
               (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x2c22c8 + cVar1 * 4))) + 0xc)));
    }
  }
  return puVar2;
}


RT_GUEST_FN(0x0023db00, guest::GetSlotDataPtr)

}  // namespace guest
