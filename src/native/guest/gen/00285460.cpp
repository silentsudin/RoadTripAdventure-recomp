// guest:0x00285460 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gsSyncVSetFlagIntc(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(32);
uint32_t auStack_20 = rtguest::allocFrame(8);
    uint32_t uStack_18 = rtguest::allocFrame(4);
  
  (*(uint32_t*)rtguest::G((uint32_t)auStack_20 + (uint32_t)(0) * 4)) = 0;
  SetVSyncFlag(auStack_20,(uint32_t)auStack_20 | 8);
  REG_INTC_STAT = 4;
  // pseudocode: return uStack_18 (uninitialized local -> arena-zero 0, deterministic)
  return (*(uint32_t*)rtguest::G((uint32_t)uStack_18));
}


RT_GUEST_FN(0x00285460, guest::gsSyncVSetFlagIntc)

}  // namespace guest
