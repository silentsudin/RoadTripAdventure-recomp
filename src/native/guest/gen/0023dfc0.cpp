// guest:0x0023dfc0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SubmitMessage2B(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  FindIndexByMaskedByte(0x50,0x2b,param_1);
  return 0;
}


RT_GUEST_FN(0x0023dfc0, guest::SubmitMessage2B)

}  // namespace guest
