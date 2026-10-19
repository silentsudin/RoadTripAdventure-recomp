// guest:0x0024f558 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiArenaInit(uint32_t param_1, uint32_t param_2, ...) {
  if ((*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int32_t))) == 0) {
    (*(int32_t*)rtguest::G((uint32_t)param_1)) = param_2 + -0x10;
    (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t))) = 0;
    return 0;
  }
  (*(int32_t*)rtguest::G((uint32_t)param_1)) = 0;
}


RT_GUEST_FN(0x0024f558, guest::uiArenaInit)

}  // namespace guest
