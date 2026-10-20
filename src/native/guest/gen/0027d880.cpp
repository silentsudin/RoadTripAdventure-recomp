// guest:0x0027d880 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __unpack_d(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(0);
  ulong uVar1;
  ulong uVar2;
  ulong uVar3;
  
  uVar1 = (*(uint64_t*)rtguest::G((uint32_t)param_1));
  uVar3 = uVar1 >> 0x34 & 0x7ff;
  uVar2 = uVar1 & 0xfffffffffffff;
  (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(uint32_t))) = (uint32_t)(uVar1 >> 0x3f);
  if (uVar3 == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_2)) = 2;
    return 0;
  }
  if (uVar3 != 0x7ff) {
    (*(uint64_t*)rtguest::G((uint32_t)(param_2 + 4 * sizeof(uint32_t)))) = uVar2 << 8 | 0x1000000000000000;
    (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(uint32_t))) = (int)uVar3 + -0x3ff;
    (*(uint32_t*)rtguest::G((uint32_t)param_2)) = 3;
    return 0;
  }
  if (uVar2 == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_2)) = 4;
    return 0;
  }
  if ((uVar1 & 0x8000000000000) == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_2)) = 0;
  }
  else {
    (*(uint32_t*)rtguest::G((uint32_t)param_2)) = 1;
  }
  (*(uint64_t*)rtguest::G((uint32_t)(param_2 + 4 * sizeof(uint32_t)))) = uVar2;
  return 0;
}


RT_GUEST_FN(0x0027d880, guest::__unpack_d)

}  // namespace guest
