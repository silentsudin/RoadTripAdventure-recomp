// guest:0x0024fad0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawOptionSlider(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  uiDrawBar3(param_1 + 0x14,param_2,param_3 + -0x14);
  if (param_4 != 0) {
    uiDrawArrow(param_1 + -0x14,param_2);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x0024fad0, guest::DrawOptionSlider)

}  // namespace guest
