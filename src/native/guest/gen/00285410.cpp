// guest:0x00285410 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceResetttyinit(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  (*(uint32_t*)rtguest::G(0x2e913c)) = 0;
  return 0;
}


RT_GUEST_FN(0x00285410, guest::sceResetttyinit)

}  // namespace guest
