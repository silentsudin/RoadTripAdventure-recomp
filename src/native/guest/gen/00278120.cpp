// guest:0x00278120 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceCdPOffCallback(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  undefined4 uVar1;
  
  if ((*(int32_t*)rtguest::G(0x2e6f3c)) < 0) {
    PowerOffCB();
  }
  DIntr();
  uVar1 = (*(uint32_t*)rtguest::G(0x1773584));
  (*(uint32_t*)rtguest::G(0x1773584)) = param_1;
  (*(uint32_t*)rtguest::G(0x1773588)) = param_2;
  EIntr();
  return uVar1;
}


RT_GUEST_FN(0x00278120, guest::sceCdPOffCallback)

}  // namespace guest
