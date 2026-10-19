// guest:0x0023d488 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetFlaggedSlotCount(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  uint uVar1;
  
  if ((param_1 != 0) && (param_1 != 0xf)) {
    uVar1 = 1 << (param_2 & 0x1f);
    return (((*(uint32_t*)rtguest::G((uint32_t)0x1780390 + (uint32_t)(param_1 * 5 + param_3 * 0xd12) * 4)) & uVar1) != 0) +
           (((*(uint32_t*)rtguest::G((uint32_t)0x1780394 + (uint32_t)(param_1 * 5 + param_3 * 0xd12) * 4)) & uVar1) != 0) +
           (((*(uint32_t*)rtguest::G((uint32_t)0x1780398 + (uint32_t)(param_1 * 5 + param_3 * 0xd12) * 4)) & uVar1) != 0) +
           (((*(uint32_t*)rtguest::G((uint32_t)0x178039c + (uint32_t)(param_1 * 5 + param_3 * 0xd12) * 4)) & uVar1) != 0) +
           (((*(uint32_t*)rtguest::G((uint32_t)0x17803a0 + (uint32_t)(param_1 * 5 + param_3 * 0xd12) * 4)) & uVar1) != 0);
  }
  return ((*(uint32_t*)rtguest::G((uint32_t)(0x1780390u + param_1 * 5 + param_3 * 0xd12) + (uint32_t)((int)param_2 >> 5) * 4)) & 1 << (param_2 & 0x1f)
         ) != 0;
}


RT_GUEST_FN(0x0023d488, guest::GetFlaggedSlotCount)

}  // namespace guest
