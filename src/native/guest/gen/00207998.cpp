// guest:0x00207998 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiHideSprite(uint32_t param_1, ...) {
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) = 0xd;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x48))) = 0xd;
  return 0;
}


RT_GUEST_FN(0x00207998, guest::uiHideSprite)

}  // namespace guest
