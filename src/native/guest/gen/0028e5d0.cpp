// guest:0x0028e5d0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t dptoul(uint64_t param_1, ...) {
  rtguest::FrameGuard rt_fg(64);
    // __unpack_d's out-struct is contiguous: class+0, sign+4, exp+8, pad+0xc, u64 mantissa+0x10
    uint32_t uStack_40 = rtguest::allocFrame(0x18);
  undefined4 uVar1;
uint32_t auStack_20 = rtguest::allocFrame(16);
  
  (*(uint64_t*)rtguest::G((uint32_t)auStack_20 + (uint32_t)(0) * 8)) = param_1;
  __unpack_d(auStack_20,uStack_40);
  uVar1 = 0;
  if (((((*(uint32_t*)rtguest::G((uint32_t)uStack_40)) != 2) && (1 < (*(uint32_t*)rtguest::G((uint32_t)uStack_40)))) && ((*(int32_t*)rtguest::G((uint32_t)uStack_40 + (uint32_t)(1) * sizeof(int32_t))) == 0)) &&
     (uVar1 = 0xffffffff, (*(uint32_t*)rtguest::G((uint32_t)uStack_40)) != 4)) {
    uVar1 = 0;
    if ((-1 < (*(int32_t*)rtguest::G((uint32_t)uStack_40 + (uint32_t)(2) * sizeof(int32_t)))) && (uVar1 = 0xffffffff, (*(int32_t*)rtguest::G((uint32_t)uStack_40 + (uint32_t)(2) * sizeof(int32_t))) < 0x20)) {
      if ((*(int32_t*)rtguest::G((uint32_t)uStack_40 + (uint32_t)(2) * sizeof(int32_t))) < 0x3d) {
        uVar1 = (uint32_t)((*(uint64_t*)rtguest::G((uint32_t)uStack_40 + (uint32_t)(4) * sizeof(int32_t))) >> (long)(0x3c - (*(int32_t*)rtguest::G((uint32_t)uStack_40 + (uint32_t)(2) * sizeof(int32_t)))));
      }
      else {
        uVar1 = (uint32_t)((*(uint64_t*)rtguest::G((uint32_t)uStack_40 + (uint32_t)(4) * sizeof(int32_t))) << (long)((*(int32_t*)rtguest::G((uint32_t)uStack_40 + (uint32_t)(2) * sizeof(int32_t))) + -0x3c));
      }
    }
  }
  return uVar1;
}


RT_GUEST_FN(0x0028e5d0, guest::dptoul)

}  // namespace guest
