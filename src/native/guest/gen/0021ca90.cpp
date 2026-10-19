// guest:0x0021ca90 translated from Ghidra pseudocode and VU0 disassembly.
#include "../guest_vu0.h"
#include "guest_decls.inc"

namespace guest {
uint32_t UpdateObjectPhysicsState(uint32_t param_1, uint32_t param_2, ...) {
  SpawnCarAtDoor();
  float x = *static_cast<float*>(rtguest::G(param_2 + 0x90)) * *static_cast<float*>(rtguest::G(0x332c28));
  float z = *static_cast<float*>(rtguest::G(param_2 + 0x98)) * *static_cast<float*>(rtguest::G(0x332c28));
  *static_cast<int32_t*>(rtguest::G(param_2 + 0xa4)) = static_cast<int32_t>(*static_cast<float*>(rtguest::G(param_2 + 0x94)) * *static_cast<float*>(rtguest::G(0x332c28)));
  *static_cast<int32_t*>(rtguest::G(param_2 + 0xa0)) += static_cast<int32_t>(x);
  *static_cast<int32_t*>(rtguest::G(param_2 + 0xa8)) += static_cast<int32_t>(z);
  LoadCarPerformanceSpecs(param_2, param_1);
  return ResetVehicleState(param_1, param_2);
}
RT_GUEST_FN(0x0021ca90, guest::UpdateObjectPhysicsState)
}  // namespace guest
