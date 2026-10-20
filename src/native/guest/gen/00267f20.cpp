// guest:0x00267f20 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t mailCollectMaskThunk(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(96);
  MailCollectMaskGates(q0);  // tail j 267f2c
  return 0;
}


RT_GUEST_FN(0x00267f20, guest::mailCollectMaskThunk)

}  // namespace guest
