// guest:0x00277cd8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceCdCallback(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  undefined4 uVar1;
  long lVar2;
  
  lVar2 = sceCdSync(1);
  uVar1 = 0;
  if (lVar2 == 0) {
    DIntr();
    uVar1 = (*(uint32_t*)rtguest::G(0x1773580));
    (*(uint32_t*)rtguest::G(0x1773580)) = param_1;
    EIntr();
  }
  return uVar1;
}


RT_GUEST_FN(0x00277cd8, guest::sceCdCallback)

}  // namespace guest
