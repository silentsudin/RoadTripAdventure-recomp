// guest:0x002704d0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceDmaSend(uint32_t param_1, uint32_t param_2, ...) {
  int iVar1;
  
  if (((*(uint32_t*)rtguest::G((uint32_t)param_1)) & 0x100) != 0) {
    iVar1 = 0xffffff;
    do {
      if (iVar1 < 0) {
        rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x330178u)));
        if (((*(uint32_t*)rtguest::G((uint32_t)param_1)) >> 8 & 1) != 0) {
          (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1)) & 0xfffffeff;
        }
      }
      iVar1 = iVar1 + -1;
    } while (((*(uint32_t*)rtguest::G((uint32_t)param_1)) & 0x100) != 0);
  }
  if ((*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0xc) * sizeof(uint32_t))) != 0xffffffff) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0xc) * sizeof(uint32_t))) = param_2;
  }
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(8) * sizeof(uint32_t))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1)) & 0xfffffff3 | 0x105;
  return 0;
}


RT_GUEST_FN(0x002704d0, guest::sceDmaSend)

}  // namespace guest
