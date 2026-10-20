// guest:0x00241670 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t EmptyStub2(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  return 0;
}


RT_GUEST_FN(0x00241670, guest::EmptyStub2)

}  // namespace guest
