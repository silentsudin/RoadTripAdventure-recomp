// guest:0x00204f98 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gs_tasks_walker(uint32_t param_1, uint32_t param_2, ...) {
  do {
    if ((*(int8_t*)rtguest::G((uint32_t)param_1)) == '\x01') {
      rtCall((*(uint32_t*)rtguest::G((uint32_t)(param_1 + 0xc))), param_1, param_2);
      (*(int8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0x24) * sizeof(int8_t))) = '\0';
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) + 1;
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10))) != 0) {
        gs_tasks_walker((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10))),param_2);
      }
      param_1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x14)));
    }
    else {
      param_1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x14)));
    }
  } while (param_1 != (uint32_t)0x0);
  return 0;
}


RT_GUEST_FN(0x00204f98, guest::gs_tasks_walker)

}  // namespace guest
