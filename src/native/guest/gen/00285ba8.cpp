// guest:0x00285ba8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t iEnableDmac(uint32_t q0, ...) {
  _iEnableDmac();
  SYNC(0);
  return 0;
}


RT_GUEST_FN(0x00285ba8, guest::iEnableDmac)

}  // namespace guest
