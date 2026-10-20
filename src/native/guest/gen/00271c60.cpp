// guest:0x00271c60 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t charCodeToTileCoord(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  uint uVar1;
  uint uVar2;
  
  uVar2 = (param_1 & 0xffff) >> 8;
  uVar1 = param_1 & 0xff;
  if (uVar2 < 0xa0) {
    uVar2 = uVar2 - 0x71;
  }
  else {
    uVar2 = uVar2 - 0xb1;
  }
  uVar2 = (uVar2 & 0xff) * 2 + 1 & 0xff;
  if ((int)(param_1 << 0x18) < 0) {
    uVar1 = uVar1 - 1 & 0xff;
  }
  if (uVar1 < 0x9e) {
    uVar1 = uVar1 - 0x1f;
  }
  else {
    uVar1 = uVar1 - 0x7d;
    uVar2 = uVar2 + 1 & 0xff;
  }
  return uVar2 << 8 | uVar1 & 0xff;
}


RT_GUEST_FN(0x00271c60, guest::charCodeToTileCoord)

}  // namespace guest
