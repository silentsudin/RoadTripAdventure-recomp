// guest:0x00267f08 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t knockCollectMaskThunk(uint32_t q0, ...) {
  KnockCollectMaskGates();
  return 0;
}


RT_GUEST_FN(0x00267f08, guest::knockCollectMaskThunk)

}  // namespace guest
