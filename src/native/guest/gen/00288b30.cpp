// guest:0x00288b30 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _sceFsWaitS(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
  _sceFsSemInit();
  WaitSema((*(uint32_t*)rtguest::G(0x2e998c)));
  return 0;
}


RT_GUEST_FN(0x00288b30, guest::_sceFsWaitS)

}  // namespace guest
