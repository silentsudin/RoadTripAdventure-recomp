// guest:0x002354d8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetObjectField844C(uint32_t param_1, uint32_t param_2, ...) {
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x844c))) = param_2;
  return 0;
}


RT_GUEST_FN(0x002354d8, guest::SetObjectField844C)

}  // namespace guest
