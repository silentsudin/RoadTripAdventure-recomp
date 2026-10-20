// guest:0x00279c40 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int64_t sceMcMkdir(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
  long lVar1;
  
  lVar1 = RT_CALLRA(0x279c48, guest::sceMcOpen());
  if (lVar1 == 0) {
    (*(uint32_t*)rtguest::G(0x2e89b8)) = 0xb;
  }
  return lVar1;
}


RT_GUEST_FN(0x00279c40, guest::sceMcMkdir)

}  // namespace guest
