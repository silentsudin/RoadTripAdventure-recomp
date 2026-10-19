// guest:0x0027cb58 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t search_fdes(uint32_t param_1, uint32_t param_2, ...) {
  int iVar1;
  
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)param_1));
  while( true ) {
    if (iVar1 == 0) {
      return (uint32_t)0x0;
    }
    if ((((*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t))) != 0) && ((*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int32_t))) != 0)) &&
       ((uint32_t)(param_2 - (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int32_t)))) < (uint32_t)(*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(int32_t))))) break;
    param_1 = (uint32_t)((int)param_1 + iVar1 + 4);
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)param_1));
  }
  return param_1;
}


RT_GUEST_FN(0x0027cb58, guest::search_fdes)

}  // namespace guest
