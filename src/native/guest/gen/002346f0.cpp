// guest:0x002346f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetTrackData(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x849c))) = param_2;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8490))) = param_3;
  return 0;
}


RT_GUEST_FN(0x002346f0, guest::SetTrackData)

}  // namespace guest
