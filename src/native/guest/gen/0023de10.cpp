// guest:0x0023de10 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t IsSlotActive(uint32_t param_1, ...) {
  bool bVar1;
  
  bVar1 = false;
  if ((long)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x23))) == (long)(*(int32_t*)rtguest::G(0x335734))) {
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x21))) == '\0';
  }
}


RT_GUEST_FN(0x0023de10, guest::IsSlotActive)

}  // namespace guest
