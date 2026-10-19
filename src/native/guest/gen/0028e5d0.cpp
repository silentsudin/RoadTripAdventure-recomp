// guest:0x0028e5d0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t dptoul(uint32_t param_1, ...) {
    uint32_t iStack_38 = rtguest::allocFrame(4);
    uint32_t iStack_3c = rtguest::allocFrame(4);
    uint32_t uStack_30 = rtguest::allocFrame(8);
    uint32_t uStack_40 = rtguest::allocFrame(4);
  undefined4 uVar1;
uint32_t auStack_20 = rtguest::allocFrame(16);
  
  (*(uint64_t*)rtguest::G((uint32_t)auStack_20 + (uint32_t)(0) * 8)) = param_1;
  __unpack_d(auStack_20,uStack_40);
  uVar1 = 0;
  if (((((*(uint32_t*)rtguest::G((uint32_t)uStack_40)) != 2) && (1 < (*(uint32_t*)rtguest::G((uint32_t)uStack_40)))) && ((*(int32_t*)rtguest::G((uint32_t)iStack_3c)) == 0)) &&
     (uVar1 = 0xffffffff, (*(uint32_t*)rtguest::G((uint32_t)uStack_40)) != 4)) {
    uVar1 = 0;
    if ((-1 < (*(int32_t*)rtguest::G((uint32_t)iStack_38))) && (uVar1 = 0xffffffff, (*(int32_t*)rtguest::G((uint32_t)iStack_38)) < 0x20)) {
      if ((*(int32_t*)rtguest::G((uint32_t)iStack_38)) < 0x3d) {
        uVar1 = (uint32_t)((*(uint64_t*)rtguest::G((uint32_t)uStack_30)) >> (long)(0x3c - (*(int32_t*)rtguest::G((uint32_t)iStack_38))));
      }
      else {
        uVar1 = (uint32_t)((*(uint64_t*)rtguest::G((uint32_t)uStack_30)) << (long)((*(int32_t*)rtguest::G((uint32_t)iStack_38)) + -0x3c));
      }
    }
  }
}


RT_GUEST_FN(0x0028e5d0, guest::dptoul)

}  // namespace guest
