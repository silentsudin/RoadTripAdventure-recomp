// guest:0x0028cfa8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t EIntr(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  EI();
  return (Status & 0x10000) != 0;
}


RT_GUEST_FN(0x0028cfa8, guest::EIntr)

}  // namespace guest
