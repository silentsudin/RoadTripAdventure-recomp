// guest:0x0024f530 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawTextHalfY(uint64_t param_1, uint32_t param_2, uint64_t param_3, uint32_t param_4, ...) {
  uiDrawText(param_1,param_2 / 2,param_3,param_4);
  return 0;
}


RT_GUEST_FN(0x0024f530, guest::DrawTextHalfY)

}  // namespace guest
