// guest:0x00287218 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifExitCmd(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
  RT_CALLRA(0x287220, guest::disableDmac(5));
  RT_CALLRA(0x287230, guest::RemoveDmacHandler(5,(*(uint32_t*)rtguest::G(0x1776e94))));
  (*(int32_t*)rtguest::G(0x2e98fc)) = 0;
  return 0;
}


RT_GUEST_FN(0x00287218, guest::sceSifExitCmd)

}  // namespace guest
