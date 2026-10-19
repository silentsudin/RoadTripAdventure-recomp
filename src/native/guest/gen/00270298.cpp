// guest:0x00270298 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceDmaPutEnv(uint32_t param_1, ...) {
  uint uVar1;
  
  if (9 < (*(int8_t*)rtguest::G((uint32_t)param_1))) {
    return 0xffffffff;
  }
  if (9 < (*(int8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int8_t)))) {
    return 0xfffffffe;
  }
  if (9 < (*(int8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int8_t)))) {
    return 0xfffffffd;
  }
  if (6 < (*(int8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(int8_t)))) {
    return 0xfffffffc;
  }
  uVar1 = REG_DMAC_CTRL & 0xffffffcf | (uint32_t)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x2dce28 + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)param_1))) * 4)) << 4;
  if ((*(int8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(int8_t))) == 0) {
    REG_DMAC_CTRL =
         uVar1 & 0xffffff31 | (uint32_t)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x2dce38 + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int8_t)))) * 4)) << 6 |
         (uint32_t)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x2dce48 + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int8_t)))) * 4)) << 2;
  }
  else {
    REG_DMAC_CTRL =
         (uVar1 & 0xffffff33 | (uint32_t)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x2dce38 + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int8_t)))) * 4)) << 6 |
         (uint32_t)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x2dce48 + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int8_t)))) * 4)) << 2) & 0xfffffcff | 2 | ((*(int8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(int8_t))) - 1) * 0x100;
  }
  REG_DMAC_PCR = CONCAT22((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 4))),(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 6))));
  REG_DMAC_SQWC = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8)));
  REG_DMAC_RBOR = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xc)));
  REG_DMAC_RBSR = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10)));
  (*(uint64_t*)rtguest::G(0x2dce58)) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)param_1));
  (*(uint32_t*)rtguest::G(0x2dce60)) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8)));
  (*(uint32_t*)rtguest::G(0x2dce68)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10)));
  return 0;
}


RT_GUEST_FN(0x00270298, guest::sceDmaPutEnv)

}  // namespace guest
