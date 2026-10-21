// guest:0x0028c990 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _sceSifLoadElfPart(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint64_t param_4, ...) {
  rtguest::FrameGuard rt_fg(112);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x60) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x64) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x20, 8);
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(2, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(3, (uint32_t)param_4);  // walk: = param_4
    rtguest::setS32(4, (uint32_t)param_3);  // walk: = param_3
  }
  undefined4 uVar1;
  long lVar2;
  
  lVar2 = RT_CALLRA(0x28c9b8, guest::_lf_bind());
  uVar1 = 0xffff0000;
  if (-1 < lVar2) {
    lVar2 = RT_CALLRA(0x28c9c8, guest::lfVersionCheck());
    if (lVar2 == 0) {
      (uint32_t)(uintptr_t)(::strncpy((char *)rtguest::G((uint32_t)((uint32_t)0x01779e08)), (char *)rtguest::G((uint32_t)(param_1)), 0xfc));
      (*(uint8_t*)rtguest::G(0x1779f03)) = 0;
      (uint32_t)(uintptr_t)(::strncpy((char *)rtguest::G((uint32_t)(0x01779f04)), (char *)rtguest::G((uint32_t)(param_2)), 0xfc));
      (*(uint8_t*)rtguest::G(0x1779fff)) = 0;
      lVar2 = RT_CALLRA(0x28ca38, guest::sceSifCallRpc(0x177a000,param_4,0,0x1779e00,0x200,0x1779e00,0x10,0));
      if (lVar2 < 0) {
        uVar1 = 0xfffeffff;
      }
      else if ((*(uint32_t*)rtguest::G(0x1779e00)) == 0) {
        uVar1 = 0xfffefffd;
      }
      else {
        (*(int32_t*)rtguest::G((uint32_t)param_3)) = (*(uint32_t*)rtguest::G(0x1779e00));
        uVar1 = 0;
        (*(int32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(int32_t))) = (*(uint32_t*)rtguest::G(0x1779e04));
      }
    }
    else {
      uVar1 = 0xfffefffc;
    }
  }
  return uVar1;
}


RT_GUEST_FN(0x0028c990, guest::_sceSifLoadElfPart)

}  // namespace guest
