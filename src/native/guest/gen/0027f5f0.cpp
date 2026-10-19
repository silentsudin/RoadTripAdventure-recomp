// guest:0x0027f5f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __sprint_r(uint32_t param_1, uint32_t param_2, ...) {
  undefined8 uVar1;
  
  uVar1 = 0;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 8))) == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 4))) = 0;
  }
  else {
    uVar1 = __sfvwrite_r();
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 4))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 8))) = 0;
  }
}


RT_GUEST_FN(0x0027f5f0, guest::__sprint_r)

}  // namespace guest
