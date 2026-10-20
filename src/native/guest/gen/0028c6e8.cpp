// guest:0x0028c6e8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifLoadModuleBuffer(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  _sceSifLoadModuleBuffer();
  return 0;
}


RT_GUEST_FN(0x0028c6e8, guest::sceSifLoadModuleBuffer)

}  // namespace guest
