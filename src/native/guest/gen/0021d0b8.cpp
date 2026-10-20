// guest:0x0021d0b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t AiCarControlDefault(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(32);
  return 0;
}


RT_GUEST_FN(0x0021d0b8, guest::AiCarControlDefault)

}  // namespace guest
