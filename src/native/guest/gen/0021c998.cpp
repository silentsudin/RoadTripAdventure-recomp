// guest:0x0021c998 translated from Ghidra pseudocode and VU0 disassembly.
#include "../guest_vu0.h"
#include "guest_decls.inc"

namespace guest {
uint32_t ResetVehicleState(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  using namespace rtguest::vu0;
  sqc2(param_2 + 0xf0, S.vf[0]);
  *static_cast<uint8_t*>(rtguest::G(param_2 + 0x1ff)) = 1;
  *static_cast<uint32_t*>(rtguest::G(param_2 + 0x1c8)) = 0;
  *static_cast<uint32_t*>(rtguest::G(param_2 + 0x1c4)) = 0;
  *static_cast<uint32_t*>(rtguest::G(param_2 + 0x1c0)) = 0;
  *static_cast<uint8_t*>(rtguest::G(param_2 + 0x1fe)) = 0;
  *static_cast<uint16_t*>(rtguest::G(param_2 + 0x1d0)) = 0;
  *static_cast<uint16_t*>(rtguest::G(param_2 + 0x1ce)) = 0;
  *static_cast<uint16_t*>(rtguest::G(param_2 + 0x1d2)) = 0;
  uint16_t flags = *static_cast<uint16_t*>(rtguest::G(param_2 + 0x198));
  if (*static_cast<int16_t*>(rtguest::G(param_2 + 0x20e)) != 0 && (flags & 3) != 0)
    PlayUiSound((static_cast<int16_t>(flags) & 1) + 0x8010);
  *static_cast<uint32_t*>(rtguest::G(param_2 + 0x100)) = 0;
  *static_cast<uint32_t*>(rtguest::G(param_2 + 0x208)) = 0;
  *static_cast<uint32_t*>(rtguest::G(param_2 + 0x1f0)) = 0;
  *static_cast<uint32_t*>(rtguest::G(param_2 + 0x1ec)) = 0;
  *static_cast<uint32_t*>(rtguest::G(param_2 + 0x1e8)) = 0;
  *static_cast<uint32_t*>(rtguest::G(param_2 + 0x10c)) = 0;
  *static_cast<uint32_t*>(rtguest::G(param_2 + 0x108)) = 0;
  *static_cast<uint32_t*>(rtguest::G(param_2 + 0x104)) = 0;
  *static_cast<uint32_t*>(rtguest::G(param_2 + 0x1dc)) = 0x1000;
  *static_cast<uint32_t*>(rtguest::G(param_2 + 0x1e4)) = 0x1000;
  *static_cast<uint32_t*>(rtguest::G(param_2 + 0x1e0)) = 0x1000;
  *static_cast<uint16_t*>(rtguest::G(param_2 + 0x20e)) = 0;
  *static_cast<uint32_t*>(rtguest::G(0x335728 + (flags & 1) * 4)) = 0;
  return 0;
}
RT_GUEST_FN(0x0021c998, guest::ResetVehicleState)
}  // namespace guest
