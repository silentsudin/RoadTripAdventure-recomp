// guest:0x0026d5e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ContinueRaceEntityLoad(uint32_t param_1, ...) {
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x24))) != '\0') {
    gs_tasks_spawn(param_1,0x25bcf0,1);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xc))) = (*(uint8_t*)rtguest::G(0x26d628));
  }
  return 0;
}


RT_GUEST_FN(0x0026d5e0, guest::ContinueRaceEntityLoad)

}  // namespace guest
