// guest:0x0023d548 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t TestFlaggedSlotBit(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
  if ((param_1 != 0) && (param_1 != 0xf)) {
    return (*(uint32_t*)rtguest::G((uint32_t)0x17803a0 + (uint32_t)(param_1 * 5 + param_3 * 0xd12) * 4)) & 1 << (param_2 & 0x1f);
  }
  return (*(uint32_t*)rtguest::G((uint32_t)0x1780390 + (uint32_t)(param_1 * 5 + param_3 * 0xd12 + ((int)param_2 >> 5)) * 4)) &
         1 << (param_2 & 0x1f);
}


RT_GUEST_FN(0x0023d548, guest::TestFlaggedSlotBit)

}  // namespace guest
