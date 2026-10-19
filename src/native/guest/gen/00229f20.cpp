// guest:0x00229f20 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t InitTuningConstants(uint32_t param_1, ...) {
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x12e8))) = 0x40;
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x12fa))) = 0x100;
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x12ea))) = 0x80;
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x12ec))) = 0x20;
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x12ee))) = 2;
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x12f0))) = 4;
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x12f2))) = 1;
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x12f6))) = 0x10;
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x12f8))) = 8;
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x12f4))) = 8;
  return 0;
}


RT_GUEST_FN(0x00229f20, guest::InitTuningConstants)

}  // namespace guest
