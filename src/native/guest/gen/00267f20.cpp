// guest:0x00267f20 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t mailCollectMaskThunk(uint32_t q0, ...) {
  MailCollectMaskGates();
  return 0;
}


RT_GUEST_FN(0x00267f20, guest::mailCollectMaskThunk)

}  // namespace guest
