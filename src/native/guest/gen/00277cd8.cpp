// guest:0x00277cd8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceCdCallback(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
  }
  undefined4 uVar1;
  long lVar2;
  
  lVar2 = RT_CALLRA(0x277cec, guest::sceCdSync(1));
  uVar1 = 0;
  if (lVar2 == 0) {
    RT_CALLRA(0x277cfc, guest::DIntr());
    uVar1 = (*(uint32_t*)rtguest::G(0x1773580));
    (*(uint32_t*)rtguest::G(0x1773580)) = param_1;
    RT_CALLRA(0x277d0c, guest::EIntr());
  }
  return uVar1;
}


RT_GUEST_FN(0x00277cd8, guest::sceCdCallback)

}  // namespace guest
