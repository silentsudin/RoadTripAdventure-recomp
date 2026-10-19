// guest:0x0028aff8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceChdir(uint32_t param_1, ...) {
  _sceCallCode(param_1,0x12);
  return 0;
}


RT_GUEST_FN(0x0028aff8, guest::sceChdir)

}  // namespace guest
