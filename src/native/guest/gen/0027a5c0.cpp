// guest:0x0027a5c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t mceStorePwd(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::gS[2] = 0x0000000001770000ull;  // walk const
  }
  size_t sVar1;
  
  if (param_1 != 0) {
    sVar1 = ::strlen((char *)rtguest::G((uint32_t)(0x21775100)));
    if (sVar1 < 0x400) {
      sVar1 = ::strlen((char *)rtguest::G((uint32_t)(0x21775100)));
    }
    else {
      sVar1 = 0x3ff;
    }
    (uint32_t)(uintptr_t)(::memcpy((char *)rtguest::G((uint32_t)((uint32_t)param_1)), (char *)rtguest::G((uint32_t)(0x21775100)), sVar1));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)(uint32_t)param_1 + (int)sVar1))) = 0;
  }
  return 0;
}


RT_GUEST_FN(0x0027a5c0, guest::mceStorePwd)

}  // namespace guest
