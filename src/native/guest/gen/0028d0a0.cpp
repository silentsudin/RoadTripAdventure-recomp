// guest:0x0028d0a0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceDeci2ReqSend(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = (uint32_t)(int32_t)param_1;
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
    uint32_t uStack_20 = rtguest::allocFrame(4);
    uint32_t iStack_1c = rtguest::allocFrame(4);
  
  (*(int32_t*)rtguest::G((uint32_t)iStack_1c)) = (int)param_2;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_20)) = param_1;
  RT_CALLRA(0x28d0bc, guest::Deci2Call(3,uStack_20));
  return 0;
}


RT_GUEST_FN(0x0028d0a0, guest::sceDeci2ReqSend)

}  // namespace guest
