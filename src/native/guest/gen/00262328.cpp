// guest:0x00262328 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t initRouletteMachineState(uint32_t param_1, ...) {
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8008))) = 0x15;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x800c))) = (*(uint32_t*)rtguest::G(0x177fdb4));
  return 0;
}


RT_GUEST_FN(0x00262328, guest::initRouletteMachineState)

}  // namespace guest
