// guest:0x0028cf60 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DIntr(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(80);
  if ((Status & 0x10000) != 0) {
    do {
      DI();
      SYNC(0x10);
    } while ((Status & 0x10000) != 0);
    return (Status & 0x10000) != 0;
  }
  return false;
}


RT_GUEST_FN(0x0028cf60, guest::DIntr)

}  // namespace guest
