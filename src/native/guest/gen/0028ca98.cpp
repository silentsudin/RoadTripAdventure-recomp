// guest:0x0028ca98 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifLoadElfPart(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
  _sceSifLoadElfPart();
  return 0;
}


RT_GUEST_FN(0x0028ca98, guest::sceSifLoadElfPart)

}  // namespace guest
