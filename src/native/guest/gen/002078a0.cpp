// guest:0x002078a0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiSetSpriteClamp(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, ...) {
  (*(uint64_t*)rtguest::G((uint32_t)param_1)) = (*(uint64_t*)rtguest::G((uint32_t)param_1)) & 0xfffff00000000000 | 10 | ((long)param_2 & 0x3ffU) << 4 |
             (long)(int)(((int)param_2 + (int)param_4) - 1U & 0x3ff) << 0xe |
             ((long)param_3 & 0x3ffU) << 0x18 |
             (long)(int)(((int)param_3 + (int)param_5) - 1U & 0x3ff) << 0x22;
  return 0;
}


RT_GUEST_FN(0x002078a0, guest::uiSetSpriteClamp)

}  // namespace guest
