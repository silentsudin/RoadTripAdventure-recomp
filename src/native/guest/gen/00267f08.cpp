// guest:0x00267f08 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t knockCollectMaskThunk(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  KnockCollectMaskGates(q0);  // tail j 267f14
  return 0;
}


RT_GUEST_FN(0x00267f08, guest::knockCollectMaskThunk)

}  // namespace guest
