// guest:0x0022e780 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawSlotScoreNumber(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  DrawDialogTextLine(param_1,param_2 * 2 + -6,(*(uint32_t*)rtguest::G((uint32_t)0x177fdb4 + (uint32_t)(param_3 * 0xd12) * 4)),
                     0x0177fd9f + param_3 * 0xd12);
  return 0;
}


RT_GUEST_FN(0x0022e780, guest::DrawSlotScoreNumber)

}  // namespace guest
