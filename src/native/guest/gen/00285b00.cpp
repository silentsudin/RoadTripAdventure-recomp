// guest:0x00285b00 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint64_t enableDmac(uint64_t param_1, ...) {
  rtguest::FrameGuard rt_fg(48);
  undefined8 uVar1;
  uint uVar2;
  
  uVar2 = Status & 0x10000;
  if (uVar2 != 0) {
    DIntr();
  }
  uVar1 = _EnableDmac(param_1);
  SYNC(0);
  if (uVar2 != 0) {
    EIntr();
  }
  return uVar1;
}


RT_GUEST_FN(0x00285b00, guest::enableDmac)

}  // namespace guest
