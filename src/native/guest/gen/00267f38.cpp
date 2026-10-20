// guest:0x00267f38 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t emptyTaskStub(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(96);
  return 0;
}


RT_GUEST_FN(0x00267f38, guest::emptyTaskStub)

}  // namespace guest
