// guest:0x00278120 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceCdPOffCallback(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    rtguest::setS32(2, (uint32_t)param_1);  // walk: = param_1
  }
  undefined4 uVar1;
  
  if ((*(int32_t*)rtguest::G(0x2e6f3c)) < 0) {
    RT_CALLRA(0x278148, guest::PowerOffCB());
  }
  RT_CALLRA(0x278150, guest::DIntr());
  uVar1 = (*(uint32_t*)rtguest::G(0x1773584));
  (*(uint32_t*)rtguest::G(0x1773584)) = param_1;
  (*(uint32_t*)rtguest::G(0x1773588)) = param_2;
  RT_CALLRA(0x278168, guest::EIntr());
  return uVar1;
}


RT_GUEST_FN(0x00278120, guest::sceCdPOffCallback)

}  // namespace guest
