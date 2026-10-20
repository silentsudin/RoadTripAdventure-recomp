// guest:0x0024fb80 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t InitCameraTransforms(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc) = 0x00000000u;
  }
  RT_CALLRA(0x24fb8c, guest::ResetVehicleState());
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x1d6))) = 0;
  vu0UnitMatrix(param_2);
  vu0UnitMatrix((int)param_2 + 0x40);
  return 0;
}


RT_GUEST_FN(0x0024fb80, guest::InitCameraTransforms)

}  // namespace guest
