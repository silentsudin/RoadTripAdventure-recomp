// guest:0x002261a0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t updateLightDirection(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
  }
    uint32_t fStack_20 = rtguest::allocFrame(4);
    uint32_t fStack_1c = rtguest::allocFrame(4);
    uint32_t fStack_18 = rtguest::allocFrame(4);
  
  (*(float*)rtguest::G((uint32_t)fStack_20)) = (*(float*)rtguest::G(0x177f5e0)) - (*(float*)rtguest::G((uint32_t)param_1));
  (*(float*)rtguest::G((uint32_t)fStack_1c)) = (*(float*)rtguest::G(0x177f5f0)) - (*(float*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(float)));
  (*(float*)rtguest::G((uint32_t)fStack_18)) = (*(float*)rtguest::G(0x177f600)) - (*(float*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(float)));
  if ((((*(float*)rtguest::G((uint32_t)fStack_20)) != 0.0) || ((*(float*)rtguest::G((uint32_t)fStack_1c)) != 0.0)) || ((*(float*)rtguest::G((uint32_t)fStack_18)) != 0.0)) {
    RT_CALLRA(0x226218, guest::vecNormalizeVu0(fStack_20,fStack_20));
  }
  (*(float*)rtguest::G(0x177f5e8)) = (*(float*)rtguest::G((uint32_t)fStack_20));
  (*(float*)rtguest::G(0x177f608)) = (*(float*)rtguest::G((uint32_t)fStack_18));
  (*(float*)rtguest::G(0x177f5f8)) = (*(float*)rtguest::G((uint32_t)fStack_1c));
  return 0;
}


RT_GUEST_FN(0x002261a0, guest::updateLightDirection)

}  // namespace guest
