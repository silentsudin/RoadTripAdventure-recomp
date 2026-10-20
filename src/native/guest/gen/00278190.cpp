// guest:0x00278190 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _sceCd_Poff_Intr(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
  if (((*(uint32_t*)rtguest::G(0x1773584)) != (uint32_t)0x0) && ((*(uint32_t*)rtguest::G(0x2e6f24)) == 0)) {
    rtCall((*(uint32_t*)rtguest::G(0x1773584)), (*(uint32_t*)rtguest::G(0x1773588)));
  }
  return 0;
}


RT_GUEST_FN(0x00278190, guest::_sceCd_Poff_Intr)

}  // namespace guest
