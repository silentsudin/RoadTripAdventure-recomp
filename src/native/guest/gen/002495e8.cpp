// guest:0x002495e8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetVehicleRaceTime(uint32_t param_1, ...) {
  return (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x14)));
}


RT_GUEST_FN(0x002495e8, guest::GetVehicleRaceTime)

}  // namespace guest
