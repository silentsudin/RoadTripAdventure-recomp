// guest:0x0027baa8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// pseudocode: void dpsub(undefined8, undefined8); callers use the return (v0 passthrough)
uint64_t dpsub(uint64_t param_1, uint64_t param_2, ...) {
  rtguest::FrameGuard rt_fg(128);
uint32_t auStack_80 = rtguest::allocFrame(32);
uint32_t auStack_60 = rtguest::allocFrame(4);
    uint32_t uStack_5c = rtguest::allocFrame(4);
    rtguest::allocFrame(24);  // guest frame gap
uint32_t auStack_40 = rtguest::allocFrame(32);
    uint32_t uStack_20 = rtguest::allocFrame(8);
    uint32_t uStack_18 = rtguest::allocFrame(8);
  undefined8 uVar1;
  
  (*(uint64_t*)rtguest::G((uint32_t)uStack_20)) = param_1;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_18)) = param_2;
  __unpack_d(uStack_20,auStack_80);
  __unpack_d(uStack_18,auStack_60);
  (*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) = (*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) ^ 1;
  uVar1 = _fpadd_parts(auStack_80,auStack_60,auStack_40);
  return __pack_d(uVar1);  // v0 passthrough; see caller 0x280dc8 cvt
}


RT_GUEST_FN(0x0027baa8, guest::dpsub)

}  // namespace guest
