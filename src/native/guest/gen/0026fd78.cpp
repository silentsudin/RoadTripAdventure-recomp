// guest:0x0026fd78 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceGsSetDefClear(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, uint32_t param_7, uint32_t param_8, uint32_t param_9, uint32_t param_10, uint32_t param_11, ...) {
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint64_t))) = 6;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(5) * sizeof(uint64_t))) = 1;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(uint64_t))) = param_7 & 0xff | 0x3f80000000000000 | (uint32_t)param_9 << 0x10 | (param_8 & 0xff) << 8
               | (uint32_t)param_10 << 0x18;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(6) * sizeof(uint64_t))) = (long)((int)param_3 << 4) | (long)((int)param_4 << 4) << 0x10 |
               (uint64_t)param_11 << 0x20;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(9) * sizeof(uint64_t))) = 5;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(8) * sizeof(uint64_t))) = (long)(((int)param_3 + (int)param_5) * 0x10) |
               (long)(((int)param_4 + (int)param_6) * 0x10) << 0x10 | (uint64_t)param_11 << 0x20;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0xb) * sizeof(uint64_t))) = 0x47;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint64_t))) = 0x47;
  (*(uint64_t*)rtguest::G((uint32_t)param_1)) = 0x30000;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint64_t))) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(7) * sizeof(uint64_t))) = 5;
  if ((long)(int)param_2 == 0) {
    (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(10) * sizeof(uint64_t))) = 0x30000;
  }
  else {
    (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(10) * sizeof(uint64_t))) = ((long)(int)param_2 & 3U) << 0x11 | 0x10000;
  }
  SYNC(0);
  return 6;
}


RT_GUEST_FN(0x0026fd78, guest::sceGsSetDefClear)

}  // namespace guest
