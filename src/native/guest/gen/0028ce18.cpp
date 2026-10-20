// guest:0x0028ce18 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifSyncIop(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
  bool bVar1;
  ulong uVar2;
  
  uVar2 = sceSifGetReg(4);
  bVar1 = (uVar2 & 0x40000) != 0;
  if (bVar1) {
    sceResetttyinit();
  }
  return bVar1;
}


RT_GUEST_FN(0x0028ce18, guest::sceSifSyncIop)

}  // namespace guest
