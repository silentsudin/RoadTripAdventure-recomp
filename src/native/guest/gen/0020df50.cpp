// guest:0x0020df50 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t getExtraPacketArea(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  int iVar1;
  
  iVar1 = param_1;
  if (param_1 < 0) {
    iVar1 = param_1 + 0xf;
  }
  if (0x180000 < param_1) {
    fatal(0x2ea450,iVar1 >> 4);
  }
  return 0x15a7b80;
}


RT_GUEST_FN(0x0020df50, guest::getExtraPacketArea)

}  // namespace guest
