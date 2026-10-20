// guest:0x0023d878 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t AwardHouseStampIfUnset(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  if ((param_1 < 10) && ((*(uint32_t*)rtguest::G((uint32_t)0x1780338 + (uint32_t)((int)param_1) * 4)) == 0)) {
    gs_stamps_award_function((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x2a2b20 + (uint32_t)((int)param_1) * 4)));
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x0023d878, guest::AwardHouseStampIfUnset)

}  // namespace guest
