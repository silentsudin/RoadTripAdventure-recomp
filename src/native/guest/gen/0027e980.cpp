// guest:0x0027e980 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __malloc_unlock(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
  (*(int32_t*)rtguest::G(0x2e9124)) = (*(int32_t*)rtguest::G(0x2e9124)) + -1;
  if ((*(int32_t*)rtguest::G(0x2e9124)) == 0) {
    (*(int32_t*)rtguest::G(0x2e9120)) = 0xffffffff;
    SignalSema((*(uint32_t*)rtguest::G(0x2e99a8)));
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x0027e980, guest::__malloc_unlock)

}  // namespace guest
