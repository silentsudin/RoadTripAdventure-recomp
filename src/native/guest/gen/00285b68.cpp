// guest:0x00285b68 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t iEnableIntc(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
  _iEnableIntc();
  SYNC(0);
  return 0;
}


RT_GUEST_FN(0x00285b68, guest::iEnableIntc)

}  // namespace guest
