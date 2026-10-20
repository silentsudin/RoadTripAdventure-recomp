// guest:0x002495b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetVehicleGear(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(0);
  return (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1ff)));
}


RT_GUEST_FN(0x002495b8, guest::GetVehicleGear)

}  // namespace guest
