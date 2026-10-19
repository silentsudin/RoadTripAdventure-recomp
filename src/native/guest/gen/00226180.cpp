// guest:0x00226180 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetSceneCamLookatIndex(uint32_t param_1, ...) {
  if (param_1 != 1) {
    return 0x0029e9b0;
  }
  return 0x0029e950;
}


RT_GUEST_FN(0x00226180, guest::GetSceneCamLookatIndex)

}  // namespace guest
