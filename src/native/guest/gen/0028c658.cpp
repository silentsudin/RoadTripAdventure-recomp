// guest:0x0028c658 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifSearchModuleByAddress(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x20, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
  }
  undefined4 uVar1;
  long lVar2;
  
  lVar2 = RT_CALLRA(0x28c668, guest::_lf_bind());
  uVar1 = 0xffff0000;
  if (-1 < lVar2) {
    lVar2 = RT_CALLRA(0x28c678, guest::lfVersionCheck());
    if (lVar2 == 0) {
      (*(uint32_t*)rtguest::G(0x1779e00)) = param_1;
      lVar2 = RT_CALLRA(0x28c6bc, guest::sceSifCallRpc(0x177a000,10,0,0x1779e00,4,0x1779e00,4,0));
      uVar1 = (*(uint32_t*)rtguest::G(0x1779e00));
      if (lVar2 < 0) {
        uVar1 = 0xfffeffff;
      }
    }
    else {
      uVar1 = 0xfffefffc;
    }
  }
  return uVar1;
}


RT_GUEST_FN(0x0028c658, guest::sceSifSearchModuleByAddress)

}  // namespace guest
