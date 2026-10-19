// guest:0x00285930 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifInvalidateDCache(uint32_t param_1, uint32_t param_2, ...) {
  uint uVar1;
  
  uVar1 = Status & 0x10000;
  if (uVar1 != 0) {
    DIntr();
  }
  _sceIDC(param_1 & 0xffffffffffffffc0,param_2 & 0xffffffffffffffc0);
  if (uVar1 != 0) {
    EIntr();
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x00285930, guest::sceSifInvalidateDCache)

}  // namespace guest
