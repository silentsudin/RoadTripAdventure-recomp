// guest:0x0028c708 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifLoadStartModuleBuffer(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
  _sceSifLoadModuleBuffer();
  return 0;
}


RT_GUEST_FN(0x0028c708, guest::sceSifLoadStartModuleBuffer)

}  // namespace guest
