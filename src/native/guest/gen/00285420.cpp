// guest:0x00285420 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gsSyncVClearIntc(uint32_t q0, ...) {
  REG_INTC_STAT = 4;
  return 0;
}


RT_GUEST_FN(0x00285420, guest::gsSyncVClearIntc)

}  // namespace guest
