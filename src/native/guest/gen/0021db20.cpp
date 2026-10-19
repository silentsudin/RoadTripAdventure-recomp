// guest:0x0021db20 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gifQuadTag(uint32_t param_1, ...) {
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint64_t))) = 5;
  (*(uint64_t*)rtguest::G((uint32_t)param_1)) = (*(uint64_t*)rtguest::G(0x29cbf0));
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint64_t))) = (*(uint64_t*)rtguest::G(0x29cbf8));
  return 0;
}


RT_GUEST_FN(0x0021db20, guest::gifQuadTag)

}  // namespace guest
