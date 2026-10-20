// guest:0x0028cdf0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifIsAliveIop(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  ulong uVar1;
  
  uVar1 = sceSifGetReg(4);
  return (uVar1 & 0x10000) != 0;
}


RT_GUEST_FN(0x0028cdf0, guest::sceSifIsAliveIop)

}  // namespace guest
