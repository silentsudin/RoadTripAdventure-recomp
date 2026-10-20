// guest:0x0028c050 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t lfVersionCheck(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  bool bVar2;
  
  bVar2 = false;
  iVar1 = ::memcmp((char *)rtguest::G((uint32_t)(0x0177a028)), (char *)rtguest::G((uint32_t)(0x2e9138u)), 4);
  if (iVar1 != 0) {
    iVar1 = ::memcmp((char *)rtguest::G((uint32_t)(0x0177a028)), (char *)rtguest::G((uint32_t)((*(uint32_t*)rtguest::G(0x002e99a4)))), 4);
    if (iVar1 != 0) {
      iVar1 = ::memcmp((char *)rtguest::G((uint32_t)(0x2e9138u)), (char *)rtguest::G((uint32_t)((*(uint32_t*)rtguest::G(0x002e99a4)))), 4);
      bVar2 = iVar1 != 0;
    }
  }
  return bVar2;
}


RT_GUEST_FN(0x0028c050, guest::lfVersionCheck)

}  // namespace guest
