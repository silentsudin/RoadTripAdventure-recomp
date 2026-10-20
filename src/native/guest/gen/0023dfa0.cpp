// guest:0x0023dfa0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SubmitMessage31(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  FindIndexByMaskedByte(0x1f,0x31,param_1);
  return 0;
}


RT_GUEST_FN(0x0023dfa0, guest::SubmitMessage31)

}  // namespace guest
