// guest:0x002236d8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawCarVariant0(uint32_t param_1, uint32_t param_2, ...) {
  drawCar(0,0,param_1,param_2,0);
  return 0;
}


RT_GUEST_FN(0x002236d8, guest::DrawCarVariant0)

}  // namespace guest
