// guest:0x00267db0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t hintCollectMaskThunk(uint32_t q0, ...) {
  HintCollectMaskGates(q0);  // tail j 267dbc, $4 passthrough
  return 0;
}


RT_GUEST_FN(0x00267db0, guest::hintCollectMaskThunk)

}  // namespace guest
