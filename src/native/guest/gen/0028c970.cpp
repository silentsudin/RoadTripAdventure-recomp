// guest:0x0028c970 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifLoadStartModule(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  _sceSifLoadModule();
  return 0;
}


RT_GUEST_FN(0x0028c970, guest::sceSifLoadStartModule)

}  // namespace guest
