// guest:0x00204d20 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t InitializeTask(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xc))) = param_2;
  (*(uint8_t*)rtguest::G((uint32_t)param_1)) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) = param_3;
  (*(uint8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0x27) * sizeof(uint8_t))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x2c))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 4))) = 0;
  (*(uint8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0x24) * sizeof(uint8_t))) = 0;
  (*(uint8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0x25) * sizeof(uint8_t))) = 0;
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) = 0;
  (*(uint8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0x26) * sizeof(uint8_t))) = 0;
  return 0;
}


RT_GUEST_FN(0x00204d20, guest::InitializeTask)

}  // namespace guest
