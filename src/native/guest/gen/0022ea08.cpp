// guest:0x0022ea08 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetPlayerScoreboardPtr(uint32_t param_1, ...) {
  param_1 = param_1 * 4;
  if (((*(uint32_t*)rtguest::G((uint32_t)0x178700b + (uint32_t)(param_1) * 4)) & 0xf0) == 0) {
    return 0x0177fd8c + ((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x178700b + (uint32_t)(param_1) * 4)) & 1) * 0xd12;
  }
  return 
          ((char)(*(uint32_t*)rtguest::G((uint32_t)0x1787009 + (uint32_t)(param_1) * 4)) * 0x10 +
           (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x2c22c8 + (char)(*(uint32_t*)rtguest::G((uint32_t)0x1787008 + (uint32_t)(param_1) * 4)) * 4))) + 0xc);
}


RT_GUEST_FN(0x0022ea08, guest::GetPlayerScoreboardPtr)

}  // namespace guest
