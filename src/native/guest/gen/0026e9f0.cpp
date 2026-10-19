// guest:0x0026e9f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceGsSetDefStoreImage(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, uint32_t param_7, uint32_t param_8, ...) {
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(uint32_t))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(5) * sizeof(uint32_t))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(6) * sizeof(uint32_t))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(7) * sizeof(uint32_t))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint32_t))) = 0x6008000;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1a))) = 0x53;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) = 0x13000000;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint32_t))) = 0x50000006;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 4))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 4))) & 0xfffffffffff8000 | 0x1000000000008005;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 6))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 6))) & 0xfffffffffffffff0 | 0xe;
  (*(int64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) =
       (long)(param_3 << 0x10) | (long)param_2 | ((long)(int)param_4 << 0x30) >> 0x18;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 10))) = 0x50;
  (*(int64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xc))) = (long)(param_6 << 0x10) | (long)param_5;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xe))) = 0x51;
  (*(int64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10))) = (long)param_7 | ((long)(int)param_8 << 0x30) >> 0x10;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x12))) = 0x52;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x16))) = 0x61;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x18))) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)param_1)) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x14))) = 0;
  SYNC(0);
  return 7;
}


RT_GUEST_FN(0x0026e9f0, guest::sceGsSetDefStoreImage)

}  // namespace guest
