// guest:0x0027bf88 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t litodp(uint32_t param_1, ...) {
    uint32_t iStack_28 = rtguest::allocFrame(4);
    uint32_t iStack_2c = rtguest::allocFrame(4);
    uint32_t uStack_20 = rtguest::allocFrame(8);
    uint32_t uStack_30 = rtguest::allocFrame(4);
  undefined8 uVar1;
  
  (*(int32_t*)rtguest::G((uint32_t)iStack_2c)) = -((int)param_1 >> 0x1f);
  (*(uint32_t*)rtguest::G((uint32_t)uStack_30)) = 3;
  if (param_1 == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)uStack_30)) = 2;
  }
  else {
    (*(uint64_t*)rtguest::G((uint32_t)uStack_20)) = param_1;
    if ((*(int32_t*)rtguest::G((uint32_t)iStack_2c)) != 0) {
      if (param_1 == 0xffffffff80000000) {
        return 0xc1e0000000000000;
      }
      (*(uint64_t*)rtguest::G((uint32_t)uStack_20)) = (uint32_t)-(int)param_1;
    }
    (*(int32_t*)rtguest::G((uint32_t)iStack_28)) = 0x3c;
    for (; (*(uint64_t*)rtguest::G((uint32_t)uStack_20)) < 0x1000000000000000; (*(uint64_t*)rtguest::G((uint32_t)uStack_20)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_20)) << 1) {
      (*(int32_t*)rtguest::G((uint32_t)iStack_28)) = (*(int32_t*)rtguest::G((uint32_t)iStack_28)) + -1;
    }
  }
  uVar1 = __pack_d(uStack_30);
  return uVar1;
}


RT_GUEST_FN(0x0027bf88, guest::litodp)

}  // namespace guest
