// guest:0x00285bc8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t iDisableDmac(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
  _iDisableDmac();
  SYNC(0);
  return 0;
}


RT_GUEST_FN(0x00285bc8, guest::iDisableDmac)

}  // namespace guest
