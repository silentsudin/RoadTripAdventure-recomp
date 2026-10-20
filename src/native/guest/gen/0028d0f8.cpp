// guest:0x0028d0f8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceDeci2ExRecv(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = (uint32_t)(int32_t)param_1;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = (uint32_t)(int32_t)param_2;
  }
    uint32_t uStack_20 = rtguest::allocFrame(4);
    uint32_t uStack_1c = rtguest::allocFrame(4);
    uint32_t uStack_18 = rtguest::allocFrame(4);
  
  (*(uint32_t*)rtguest::G((uint32_t)uStack_18)) = param_3 & 0xffff;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_20)) = param_1;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_1c)) = param_2;
  Deci2Call(0xfffffffffffffffb,uStack_20);
  return 0;
}


RT_GUEST_FN(0x0028d0f8, guest::sceDeci2ExRecv)

}  // namespace guest
