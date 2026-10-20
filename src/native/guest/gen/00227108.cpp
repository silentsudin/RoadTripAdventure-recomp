// guest:0x00227108 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t vifBeginPacket(uint64_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
uint32_t puVar1;
  
  (*(uint32_t*)rtguest::G(0x3356f4)) = (uint32_t)param_1;
  if (param_1 == 0) {
    if ((*(uint32_t*)rtguest::G(0x3356ec)) != (uint32_t)0x0) {
      fatal(0x2eafd8);
    }
    puVar1 = 0x01750bf0;
  }
  else {
    if ((*(uint32_t*)rtguest::G(0x3356f0)) != 0) {
      fatal(0x2eaff0);
    }
    puVar1 = 0x01730bf0;
    (*(uint32_t*)rtguest::G(0x3356f0)) = (int)(*(uint32_t*)rtguest::G(0x3356ec));
  }
  (*(uint32_t*)rtguest::G(0x3356ec)) = puVar1;
  return 0;
}


RT_GUEST_FN(0x00227108, guest::vifBeginPacket)

}  // namespace guest
