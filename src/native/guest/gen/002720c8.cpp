// guest:0x002720c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t charCodeToGlyphIndex(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(0);
  uint uVar1;
  
  uVar1 = param_1 & 0xffff;
  if ((param_1 & 0xff00) != 0) {
    if (uVar1 - 0x2421 < 0x53) {
      uVar1 = uVar1 - 0x23a1 & 0xffff;
    }
    else {
      uVar1 = 0x7f;
    }
  }
  return uVar1 & 0xff;
}


RT_GUEST_FN(0x002720c8, guest::charCodeToGlyphIndex)

}  // namespace guest
