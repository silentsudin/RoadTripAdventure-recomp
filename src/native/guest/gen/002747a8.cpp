// guest:0x002747a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t scePadSetWarningLevel(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
    rtguest::gS[0] = 0x0000000001773500ull;  // walk const
  }
  undefined4 uVar1;
  long lVar2;
  
  (*(uint32_t*)rtguest::G(0x1773500)) = 0x14;
  (*(uint32_t*)rtguest::G(0x1773504)) = param_1;
  lVar2 = RT_CALLRA(0x2747ec, guest::sceSifCallRpc(0x17731c0,1,0,0x1773500,0x80,0x1773500,0x80,0));
  uVar1 = (*(uint32_t*)rtguest::G(0x1773508));
  if (lVar2 < 0) {
    uVar1 = 0;
  }
  return uVar1;
}


RT_GUEST_FN(0x002747a8, guest::scePadSetWarningLevel)

}  // namespace guest
