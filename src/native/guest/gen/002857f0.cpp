// guest:0x002857f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifWriteBackDCache(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::setS32(1, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(2, (uint32_t)param_1);  // walk: = param_1
  }
  uint uVar1;
  
  uVar1 = Status & 0x10000;
  if (uVar1 != 0) {
    RT_CALLRA(0x285820, guest::DIntr());
  }
  RT_CALLRA(0x285834, guest::_sceSDC(param_1 & 0xffffffffffffffc0,param_2 & 0xffffffffffffffc0));
  if (uVar1 != 0) {
    EIntr();
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x002857f0, guest::sceSifWriteBackDCache)

}  // namespace guest
