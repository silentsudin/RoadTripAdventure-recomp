// guest:0x002704a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceDmaPutStallAddr(uint32_t param_1, ...) {
  undefined4 uVar1;
  
  uVar1 = REG_DMAC_STADR;
  if (param_1 != -1) {
    REG_DMAC_STADR = (int)param_1;
  }
}


RT_GUEST_FN(0x002704a8, guest::sceDmaPutStallAddr)

}  // namespace guest
