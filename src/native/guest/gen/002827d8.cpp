// guest:0x002827d8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t std(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(32);
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0x15) * sizeof(uint32_t))) = param_4;
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 3))) = param_2;
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xe))) = param_3;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(8) * sizeof(uint32_t))) = 0x284750;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(9) * sizeof(uint32_t))) = 0x2847b8;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(10) * sizeof(uint32_t))) = 0x284838;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0xb) * sizeof(uint32_t))) = 0x2848a0;
  (*(uint32_t*)rtguest::G((uint32_t)param_1)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint32_t))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(uint32_t))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(5) * sizeof(uint32_t))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(6) * sizeof(uint32_t))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(7) * sizeof(uint32_t))) = param_1;
  return 0;
}


RT_GUEST_FN(0x002827d8, guest::std)

}  // namespace guest
