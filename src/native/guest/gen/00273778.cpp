// guest:0x00273778 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t scePadEnd(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
    rtguest::gS[0] = 0x0000000001773500ull;  // walk const
  }
  int iVar1;
  long lVar2;
  
  (*(uint32_t*)rtguest::G(0x1773500)) = 0xf;
  lVar2 = RT_CALLRA(0x2737b8, guest::sceSifCallRpc(0x17731c0,1,0,0x1773500,0x80,0x1773500,0x80,0));
  if (lVar2 < 0) {
    iVar1 = 0;
  }
  else {
    iVar1 = (*(uint32_t*)rtguest::G(0x177350c));
    if ((*(uint32_t*)rtguest::G(0x177350c)) == 1) {
      (*(uint32_t*)rtguest::G(0x002e6ec0)) = 0;
    }
  }
  return iVar1;
}


RT_GUEST_FN(0x00273778, guest::scePadEnd)

}  // namespace guest
