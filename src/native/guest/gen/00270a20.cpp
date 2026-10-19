// guest:0x00270a20 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceDmaSync(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  uint uVar1;
  
  if (param_2 == 1) {
    uVar1 = (*(uint32_t*)rtguest::G((uint32_t)param_1)) >> 8 & 1;
  }
  else {
    uVar1 = (*(uint32_t*)rtguest::G((uint32_t)param_1));
    if (param_3 == 0) {
      param_3 = 0x1000000;
    }
    while ((uVar1 & 0x100) != 0) {
      param_3 = param_3 + -1;
      if (param_3 < 0) {
        ::printf((char *)rtguest::G((uint32_t)(0x330178u)));
        if (((*(uint32_t*)rtguest::G((uint32_t)param_1)) >> 8 & 1) != 0) {
          (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1)) & 0xfffffeff;
        }
      }
      uVar1 = (*(uint32_t*)rtguest::G((uint32_t)param_1));
    }
    uVar1 = 0;
  }
  return uVar1;
}


RT_GUEST_FN(0x00270a20, guest::sceDmaSync)

}  // namespace guest
