// guest:0x00233fc8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t InitHudConfig2(uint32_t param_1, uint32_t param_2, ...) {
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8078))) = 0x2800000001;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8080))) = 0x29f6a800000014;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8088))) = 2;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x80a0))) = 0xffffffff;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8090))) = 0x28;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8094))) = 0x34;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8098))) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x809c))) = param_2;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x808c))) = 1;
  return 0;
}


RT_GUEST_FN(0x00233fc8, guest::InitHudConfig2)

}  // namespace guest
