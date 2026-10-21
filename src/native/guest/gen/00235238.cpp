// guest:0x00235238 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetHudState3(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
    rtguest::gS[2] = 0x0000000000000003ull;  // walk const
  }
uint32_t __s;
  int iVar1;
  
  __s = (uint32_t)(param_1 + 0x82e0);
  iVar1 = 0;
  if (param_3 != 0) {
    iVar1 = 3;
  }
  if ((*(int32_t*)rtguest::G((uint32_t)__s)) != iVar1) {
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x2c));
    (*(int32_t*)rtguest::G((uint32_t)__s)) = iVar1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8308))) = 0x40000000;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82e4))) = 0x140;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82e8))) = 0xe0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82f0))) = 3;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82f4))) = 0x0029fac8;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82f8))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8304))) = 0x1e;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82ec))) = 1;
  }
  return 0;
}


RT_GUEST_FN(0x00235238, guest::SetHudState3)

}  // namespace guest
