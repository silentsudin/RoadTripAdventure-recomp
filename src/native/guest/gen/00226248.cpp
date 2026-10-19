// guest:0x00226248 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t setFixedLight(uint32_t param_1, uint32_t param_2, ...) {
  param_2 = param_2 * 0x10;
  (*(uint32_t*)rtguest::G(0x177f738)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0029ea18 + param_2)));
  (*(float*)rtguest::G(0x177f73c)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x29ea1c)));
  (*(uint32_t*)rtguest::G(0x177f750)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x29ea14)));
  (*(uint8_t*)rtguest::G(0x177f754)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0029ea10 + param_2)));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x50))) = (*(uint8_t*)rtguest::G(0x177f754));
  return 0;
}


RT_GUEST_FN(0x00226248, guest::setFixedLight)

}  // namespace guest
