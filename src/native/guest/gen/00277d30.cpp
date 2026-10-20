// guest:0x00277d30 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _sceCd_cd_callback(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  (*(int32_t*)rtguest::G(0x2e6f54)) = (*(int32_t*)rtguest::G((uint32_t)param_1));
  (*(uint32_t*)rtguest::G(0x2e6f58)) = (*(int32_t*)rtguest::G(0x2e6f54));
  if ((*(int32_t*)rtguest::G(0x2e6f54)) == 0xb) {
    (*(uint32_t*)rtguest::G(0x2e6f30)) = 0;
  }
  else {
    iSignalSema((*(int32_t*)rtguest::G(0x2e6f28)));
    if (((*(int32_t*)rtguest::G(0x2e6f14)) == 0) || ((*(uint32_t*)rtguest::G(0x1773580)) == 0)) {
      (*(uint32_t*)rtguest::G(0x2e6f30)) = 0;
    }
    else {
      iSignalSema((*(uint32_t*)rtguest::G(0x2e6f20)));
    }
  }
  (*(int32_t*)rtguest::G(0x2e6f54)) = 0;
  return 0;
}


RT_GUEST_FN(0x00277d30, guest::_sceCd_cd_callback)

}  // namespace guest
