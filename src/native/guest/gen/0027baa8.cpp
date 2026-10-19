// guest:0x0027baa8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t dpsub(uint32_t param_1, uint32_t param_2, ...) {
    uint32_t uStack_18 = rtguest::allocFrame(8);
    uint32_t uStack_20 = rtguest::allocFrame(8);
    uint32_t uStack_5c = rtguest::allocFrame(4);
  undefined8 uVar1;
uint32_t auStack_80 = rtguest::allocFrame(32);
uint32_t auStack_60 = rtguest::allocFrame(4);
uint32_t auStack_40 = rtguest::allocFrame(32);
  
  (*(uint64_t*)rtguest::G((uint32_t)uStack_20)) = param_1;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_18)) = param_2;
  __unpack_d(uStack_20,auStack_80);
  __unpack_d(uStack_18,auStack_60);
  (*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) = (*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) ^ 1;
  uVar1 = _fpadd_parts(auStack_80,auStack_60,auStack_40);
  __pack_d(uVar1);
  return 0;
}


RT_GUEST_FN(0x0027baa8, guest::dpsub)

}  // namespace guest
