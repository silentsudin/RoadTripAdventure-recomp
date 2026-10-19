// guest:0x00204ee0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetTaskListFlag(uint32_t param_1, ...) {
uint32_t pbVar1;
  
  pbVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10)));
  while (pbVar1 != (uint32_t)0x0) {
    (*(int8_t*)rtguest::G((uint32_t)pbVar1)) = (*(int8_t*)rtguest::G((uint32_t)pbVar1)) | 2;
    pbVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(pbVar1 + 0x14)));
  }
  return 0;
}


RT_GUEST_FN(0x00204ee0, guest::SetTaskListFlag)

}  // namespace guest
