// guest:0x0021d718 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gifBeginSpriteBatch(uint32_t param_1, uint64_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  undefined8 uVar1;
  
  (*(uint64_t*)rtguest::G((uint32_t)param_1)) = (*(uint64_t*)rtguest::G(0x29cc70));
  uVar1 = (*(uint64_t*)rtguest::G(0x29cc78));
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint64_t))) = 0x47;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(10) * sizeof(uint64_t))) = 5;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint64_t))) = uVar1;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint64_t))) = 0x5001b;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(5) * sizeof(uint64_t))) = 0x42;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(uint64_t))) = 0x44;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(7) * sizeof(uint64_t))) = 0x14;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(6) * sizeof(uint64_t))) = 0x60;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(9) * sizeof(uint64_t))) = 6;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(8) * sizeof(uint64_t))) = param_2;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0xb) * sizeof(uint64_t))) = 8;
  return 0;
}


RT_GUEST_FN(0x0021d718, guest::gifBeginSpriteBatch)

}  // namespace guest
