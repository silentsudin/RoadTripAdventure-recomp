// guest:0x00285b88 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t iDisableIntc(uint32_t q0, ...) {
  _iDisableIntc();
  SYNC(0);
  return 0;
}


RT_GUEST_FN(0x00285b88, guest::iDisableIntc)

}  // namespace guest
