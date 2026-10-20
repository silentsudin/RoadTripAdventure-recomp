// guest:0x00223490 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UploadCarColorPalette(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  vifUploadColours(0x29d2d0,0x29d2e0);
  vifDirect(0x29d2f0,2);
  return 0;
}


RT_GUEST_FN(0x00223490, guest::UploadCarColorPalette)

}  // namespace guest
