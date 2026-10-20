// guest:0x002854c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t write(uint32_t __fd, uint32_t __buf, uint32_t __n, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
  }
  ssize_t sVar1;
  long lVar2;
  
  if (__fd - 1U < 2) {
    if ((*(int32_t*)rtguest::G(0x2e913c)) == 0) {
      lVar2 = RT_CALLRA(0x2854f8, guest::sceTtyInit(__buf));
      if (lVar2 == 0) goto LAB_00285520;
      (*(int32_t*)rtguest::G(0x2e913c)) = 1;
    }
    sVar1 = RT_CALLRA(0x285510, guest::sceTtyWrite(__buf,__n));
  }
  else {
LAB_00285520:
    sVar1 = -1;
  }
  return sVar1;
}


RT_GUEST_FN(0x002854c0, guest::write)

}  // namespace guest
