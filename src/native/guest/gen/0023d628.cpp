// guest:0x0023d628 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t TestStateBitFc78(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  bool bVar1;
  uint uVar2;
  
  uVar2 = (param_1 & 0xff) + 0xff;
  if ((param_1 & 0xff) == 0) {
    bVar1 = false;
    if ((*(uint64_t*)rtguest::G(0x177fc78)) == -1) {
      return (*(uint32_t*)rtguest::G(0x177fc80)) == 0xfffffffff;
    }
  }
  else {
    bVar1 = ((*(uint64_t*)rtguest::G((uint32_t)0x177fc78 + (uint32_t)((uVar2 & 0xff) >> 6) * 8)) & 1L << ((long)(int)uVar2 & 0x3fU)) != 0;
  }
  return bVar1;
}


RT_GUEST_FN(0x0023d628, guest::TestStateBitFc78)

}  // namespace guest
