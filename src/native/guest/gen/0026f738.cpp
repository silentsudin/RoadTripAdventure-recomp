// guest:0x0026f738 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceGsSwapDBuffDc(uint32_t param_1, uint32_t param_2, ...) {
  sceGsPutDispEnv((param_2 & 1) * 0x28 + param_1);
  if ((param_2 & 1) == 0) {
    sceGsPutDrawEnv(param_1 + 0x50);
  }
  else {
    sceGsPutDrawEnv(param_1 + 0x1c0);
  }
  return 0;
}


RT_GUEST_FN(0x0026f738, guest::sceGsSwapDBuffDc)

}  // namespace guest
