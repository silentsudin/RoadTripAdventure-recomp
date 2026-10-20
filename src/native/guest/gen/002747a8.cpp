// guest:0x002747a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t scePadSetWarningLevel(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  undefined4 uVar1;
  long lVar2;
  
  (*(uint32_t*)rtguest::G(0x1773500)) = 0x14;
  (*(uint32_t*)rtguest::G(0x1773504)) = param_1;
  lVar2 = sceSifCallRpc(0x17731c0,1,0,0x1773500,0x80,0x1773500,0x80,0);
  uVar1 = (*(uint32_t*)rtguest::G(0x1773508));
  if (lVar2 < 0) {
    uVar1 = 0;
  }
  return uVar1;
}


RT_GUEST_FN(0x002747a8, guest::scePadSetWarningLevel)

}  // namespace guest
