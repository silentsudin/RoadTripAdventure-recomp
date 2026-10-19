// guest:0x0027c040 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t dptoli(uint32_t param_1, ...) {
    uint32_t iStack_38 = rtguest::allocFrame(4);
    uint32_t iStack_3c = rtguest::allocFrame(4);
    uint32_t uStack_30 = rtguest::allocFrame(8);
    uint32_t uStack_40 = rtguest::allocFrame(4);
  int iVar1;
uint32_t auStack_20 = rtguest::allocFrame(16);
  
  (*(uint64_t*)rtguest::G((uint32_t)auStack_20 + (uint32_t)(0) * 8)) = param_1;
  __unpack_d(auStack_20,uStack_40);
  if ((*(uint32_t*)rtguest::G((uint32_t)uStack_40)) == 2) {
    return 0;
  }
  if ((*(uint32_t*)rtguest::G((uint32_t)uStack_40)) < 2) {
    return 0;
  }
  if ((*(uint32_t*)rtguest::G((uint32_t)uStack_40)) != 4) {
    if ((*(int32_t*)rtguest::G((uint32_t)iStack_38)) < 0) {
      return 0;
    }
    if ((*(int32_t*)rtguest::G((uint32_t)iStack_38)) < 0x1f) {
      iVar1 = (int)((*(uint64_t*)rtguest::G((uint32_t)uStack_30)) >> (long)(0x3c - (*(int32_t*)rtguest::G((uint32_t)iStack_38))));
      if ((*(int32_t*)rtguest::G((uint32_t)iStack_3c)) == 0) {
        return iVar1;
      }
      return -iVar1;
    }
  }
  iVar1 = 0x7fffffff;
  if ((*(int32_t*)rtguest::G((uint32_t)iStack_3c)) != 0) {
    iVar1 = -0x80000000;
  }
  return iVar1;
}


RT_GUEST_FN(0x0027c040, guest::dptoli)

}  // namespace guest
