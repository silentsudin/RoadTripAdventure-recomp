// guest:0x00288c00 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _sceFs_Poff_Intr(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  if ((*(uint32_t*)rtguest::G((uint32_t)param_2)) != 0) {
    rtCall(*(uint32_t*)rtguest::G((uint32_t)param_2),
           *(uint32_t*)rtguest::G((uint32_t)(param_2 + 4)));
  }
  SYNC(0);
  EI();
  return 0;
}


RT_GUEST_FN(0x00288c00, guest::_sceFs_Poff_Intr)

}  // namespace guest
