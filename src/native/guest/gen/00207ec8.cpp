// guest:0x00207ec8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t TransformRectangle(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
  }
    uint32_t iStack_20 = rtguest::allocFrame(4);
    uint32_t iStack_1c = rtguest::allocFrame(4);
    uint32_t iStack_18 = rtguest::allocFrame(4);
    uint32_t iStack_14 = rtguest::allocFrame(4);
  
  (*(int32_t*)rtguest::G((uint32_t)iStack_1c)) = (int)(*(int16_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int16_t)));
  (*(int32_t*)rtguest::G((uint32_t)iStack_14)) = (int)(((uint32_t)(*(uint16_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(uint16_t))) + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(3) * sizeof(uint16_t)))) * 0x10000) >> 0x10;
  (*(int32_t*)rtguest::G((uint32_t)iStack_20)) = (int)(*(int16_t*)rtguest::G((uint32_t)param_2));
  (*(int32_t*)rtguest::G((uint32_t)iStack_18)) = (int)(*(int16_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int16_t)));
  RT_CALLRA(0x207f10, guest::vu0Itof4Vector(param_1,iStack_20));
  RT_CALLRA(0x207f20, guest::vu0ScaleVector((*(float*)rtguest::G(0x332b10)),param_1,param_1));
  return 0;
}


RT_GUEST_FN(0x00207ec8, guest::TransformRectangle)

}  // namespace guest
