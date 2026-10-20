// guest:0x0023d8e8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t TestFlagFc68(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  bool bVar1;
  int iVar2;
  int iVar3;
  
  if (param_1 == 0) {
    bVar1 = false;
    if ((*(uint64_t*)rtguest::G(0x177fc68)) == -1) {
      return (*(uint32_t*)rtguest::G(0x177fc70)) == 0xfffffffff;
    }
  }
  else {
    iVar2 = (int)param_1 + -1;
    iVar3 = iVar2;
    if ((long)iVar2 < 0) {
      iVar3 = (int)param_1 + 0x3e;
    }
    bVar1 = ((*(uint64_t*)rtguest::G((uint32_t)0x177fc68 + (uint32_t)(iVar3 >> 6) * 8)) & 1L << ((long)iVar2 & 0x3fU)) != 0;
  }
  return bVar1;
}


RT_GUEST_FN(0x0023d8e8, guest::TestFlagFc68)

}  // namespace guest
