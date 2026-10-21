// guest:0x00285540 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t read(uint32_t __fd, uint32_t __buf, uint32_t __nbytes, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::gS[0] = 0x00000000002f0000ull;  // walk const
  }
  ssize_t sVar1;
  long lVar2;
  
  if (__fd == 0) {
    if ((*(int32_t*)rtguest::G(0x2e913c)) == 0) {
      lVar2 = RT_CALLRA(0x285570, guest::sceTtyInit(__buf));
      if (lVar2 == 0) goto LAB_00285598;
      (*(int32_t*)rtguest::G(0x2e913c)) = 1;
    }
    sVar1 = RT_CALLRA(0x285588, guest::sceTtyRead(__buf,__nbytes));
  }
  else {
LAB_00285598:
    sVar1 = -1;
  }
  return sVar1;
}


RT_GUEST_FN(0x00285540, guest::read)

}  // namespace guest
