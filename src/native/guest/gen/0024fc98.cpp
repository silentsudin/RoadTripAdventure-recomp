// guest:0x0024fc98 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ScaleShortByMatrix(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
uint32_t auStack_30 = rtguest::allocFrame(16);
uint32_t aiStack_20 = rtguest::allocFrame(16);
  
  (*(int32_t*)rtguest::G((uint32_t)aiStack_20 + (uint32_t)(0) * 4)) = (int)param_1;
  RT_CALLRA(0x24fcb0, guest::vu0Itof4Vector(auStack_30,aiStack_20));
  RT_CALLRA(0x24fcc0, guest::vu0ScaleVector((*(float*)rtguest::G(0x332f98)),auStack_30,auStack_30));
  return (*(uint32_t*)rtguest::G((uint32_t)auStack_30 + (uint32_t)(0) * 4));;
}


RT_GUEST_FN(0x0024fc98, guest::ScaleShortByMatrix)

}  // namespace guest
