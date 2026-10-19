// guest:0x00288098 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifCheckStatRpc(uint32_t param_1, ...) {
  int iVar1;
  
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)param_1));
  if (((iVar1 != 0) && ((*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t))) == (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x18))))) &&
     (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x10))) & 1) != 0)) {
    return 1;
  }
  return 0;
}


RT_GUEST_FN(0x00288098, guest::sceSifCheckStatRpc)

}  // namespace guest
