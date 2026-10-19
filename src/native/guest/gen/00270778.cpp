// guest:0x00270778 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceDmaRecv(uint32_t param_1, ...) {
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
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(8) * sizeof(uint32_t))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1)) & 0xfffffff2 | 0x104;
  return 0;
}


RT_GUEST_FN(0x00270778, guest::sceDmaRecv)

}  // namespace guest
