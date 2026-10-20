// guest:0x0023dbf0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t TestFlagFca8Clear(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  int iVar1;
  int iVar2;
  
  if (param_1 != 0) {
    iVar1 = (int)param_1 + -1;
    iVar2 = iVar1;
    if ((long)iVar1 < 0) {
      iVar2 = (int)param_1 + 0x3e;
    }
    return ((*(uint64_t*)rtguest::G((uint32_t)0x177fca8 + (uint32_t)(iVar2 >> 6) * 8)) & 1L << ((long)iVar1 & 0x3fU)) == 0;
  }
  return (*(uint64_t*)rtguest::G(0x177fca8)) == 0 && (*(int64_t*)rtguest::G(0x177fcb0)) == 0;
}


RT_GUEST_FN(0x0023dbf0, guest::TestFlagFca8Clear)

}  // namespace guest
