// guest:0x00285540 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t read(uint32_t __fd, uint32_t __buf, uint32_t __nbytes, ...) {
  rtguest::FrameGuard rt_fg;
  ssize_t sVar1;
  long lVar2;
  
  if (__fd == 0) {
    if ((*(int32_t*)rtguest::G(0x2e913c)) == 0) {
      lVar2 = sceTtyInit(__buf);
      if (lVar2 == 0) goto LAB_00285598;
      (*(int32_t*)rtguest::G(0x2e913c)) = 1;
    }
    sVar1 = sceTtyRead(__buf,__nbytes);
  }
  else {
LAB_00285598:
    sVar1 = -1;
  }
  return sVar1;
}


RT_GUEST_FN(0x00285540, guest::read)

}  // namespace guest
