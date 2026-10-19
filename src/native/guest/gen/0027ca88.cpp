// guest:0x0027ca88 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t count_fdes(uint32_t param_1, ...) {
  int iVar1;
  int iVar2;
  
  iVar2 = 0;
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)param_1));
  while (iVar1 != 0) {
    if (((*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t))) != 0) && ((*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int32_t))) != 0)) {
      iVar2 = iVar2 + 1;
    }
    param_1 = (uint32_t)((int)param_1 + iVar1 + 4);
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)param_1));
  }
  return iVar2;
}


RT_GUEST_FN(0x0027ca88, guest::count_fdes)

}  // namespace guest
