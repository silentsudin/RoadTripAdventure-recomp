// guest:0x002860b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ttyRingPush(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(0);
  int iVar1;
  
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(int32_t)));
  (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t))) = (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t))) + 1;
  (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(int32_t))) = iVar1 + 1;
  if (iVar1 + 1 == (int)param_1 + (*(int32_t*)rtguest::G((uint32_t)param_1)) + 0x10) {
    (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(int32_t))) = (int)(param_1 + 16);
  }
  return 0;
}


// W13 fn/002860b0.c:16 `(int)(param_1 + 4)` int*: typed-pointer arithmetic is element-scaled (x4); transliterator left it byte-unscaled
RT_GUEST_FN(0x002860b0, guest::ttyRingPush)

}  // namespace guest
