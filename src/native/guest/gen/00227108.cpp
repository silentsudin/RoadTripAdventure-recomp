// guest:0x00227108 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t vifBeginPacket(uint64_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
uint32_t puVar1;
  
  (*(uint32_t*)rtguest::G(0x3356f4)) = (uint32_t)param_1;
  if (param_1 == 0) {
    if ((*(uint32_t*)rtguest::G(0x3356ec)) != (uint32_t)0x0) {
      RT_CALLRA(0x227128, guest::fatal(0x2eafd8));
    }
    puVar1 = 0x01750bf0;
  }
  else {
    if ((*(uint32_t*)rtguest::G(0x3356f0)) != 0) {
      RT_CALLRA(0x227150, guest::fatal(0x2eaff0));
    }
    puVar1 = 0x01730bf0;
    (*(uint32_t*)rtguest::G(0x3356f0)) = (int)(*(uint32_t*)rtguest::G(0x3356ec));
  }
  (*(uint32_t*)rtguest::G(0x3356ec)) = puVar1;
  return 0;
}


RT_GUEST_FN(0x00227108, guest::vifBeginPacket)

}  // namespace guest
