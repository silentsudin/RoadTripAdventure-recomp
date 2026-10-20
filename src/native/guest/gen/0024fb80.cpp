// guest:0x0024fb80 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t InitCameraTransforms(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  ResetVehicleState();
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x1d6))) = 0;
  vu0UnitMatrix(param_2);
  vu0UnitMatrix((int)param_2 + 0x40);
  return 0;
}


RT_GUEST_FN(0x0024fb80, guest::InitCameraTransforms)

}  // namespace guest
