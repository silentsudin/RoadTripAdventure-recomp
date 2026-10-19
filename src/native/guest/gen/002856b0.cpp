// guest:0x002856b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint64_t sysFstat(uint64_t param_1, uint32_t param_2, ...) {
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x48))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 4))) = 0x2000;
  return 0;
}


RT_GUEST_FN(0x002856b0, sysFstat)

}  // namespace guest
