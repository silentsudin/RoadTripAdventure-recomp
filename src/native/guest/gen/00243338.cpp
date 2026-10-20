// guest:0x00243338 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t mapDot(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x48) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4c) = 0x00000000u;
  }
uint32_t auStack_60 = rtguest::allocFrame(40);
    uint32_t uStack_38 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_30 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_28 = rtguest::allocFrame(4);
  
  RT_CALLRA(0x243350, guest::vu0UnitMatrix(auStack_60));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_28)) = (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(uint32_t)));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_30)) = (*(uint32_t*)rtguest::G((uint32_t)param_2));
  (*(uint32_t*)rtguest::G((uint32_t)auStack_60 + (uint32_t)(0) * 4)) = param_1;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_38)) = param_1;
  RT_CALLRA(0x243370, guest::vifObjectMatrix(auStack_60));
  RT_CALLRA(0x24337c, guest::vifCallModel(0x290940));
  return 0;
}


RT_GUEST_FN(0x00243338, guest::mapDot)

}  // namespace guest
