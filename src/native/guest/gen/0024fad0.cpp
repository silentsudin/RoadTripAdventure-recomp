// guest:0x0024fad0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

void DrawOptionSlider(uint32_t param_1, uint64_t param_2, uint32_t param_3, uint64_t param_4) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
  }
  RT_CALLRA(0x24faf4, guest::uiDrawBar3(param_1 + 0x14,param_2,param_3 + -0x14));
  if (param_4 != 0) {
    uiDrawArrow(param_1 + -0x14,param_2);
    return;
  }
  return;
}


RT_GUEST_FN(0x0024fad0, guest::DrawOptionSlider)

}  // namespace guest
