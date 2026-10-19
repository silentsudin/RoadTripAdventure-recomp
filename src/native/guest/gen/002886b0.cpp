// guest:0x002886b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t get_iob(uint32_t param_1, ...) {
  int iVar1;
  
  fsFdSemInit();
  WaitSema((*(uint32_t*)rtguest::G(0x2e9990)));
  if (param_1 < 0x20) {
    iVar1 = (int)param_1 * 0x10 + 0x1779980;
    SignalSema((*(uint32_t*)rtguest::G(0x2e9990)));
  }
  else {
    SignalSema((*(uint32_t*)rtguest::G(0x2e9990)));
    iVar1 = 0;
  }
  return iVar1;
}


RT_GUEST_FN(0x002886b0, guest::get_iob)

}  // namespace guest
