// guest:0x002586a0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SndRpcCmd22(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, ...) {
  rtguest::FrameGuard rt_fg(112);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x14) = (uint32_t)(int32_t)param_2;
    *(uint32_t*)rtguest::G(rt_sv+0x10) = (uint32_t)(int32_t)param_1;
    *(uint32_t*)rtguest::G(rt_sv+0x18) = (uint32_t)(int32_t)param_3;
    *(uint32_t*)rtguest::G(rt_sv+0x20) = (uint32_t)(int32_t)param_5;
  }
    uint32_t uStack_4c = rtguest::allocFrame(4);
    uint32_t uStack_48 = rtguest::allocFrame(8);
    uint32_t uStack_40 = rtguest::allocFrame(8);
    uint32_t uStack_38 = rtguest::allocFrame(8);
    uint32_t uStack_30 = rtguest::allocFrame(8);
    uint32_t uStack_28 = rtguest::allocFrame(8);
  
  if (param_4 == 0) {
    param_3 = 1;
  }
  (*(uint64_t*)rtguest::G(0x175e880)) = CONCAT44(param_2,param_1);
  (*(uint64_t*)rtguest::G(0x175e888)) = CONCAT44((int)param_4,param_3);
  (*(uint64_t*)rtguest::G(0x175e898)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_48));
  (*(uint64_t*)rtguest::G(0x175e890)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_4c)),param_5);
  (*(uint32_t*)rtguest::G(0x175e8a0)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_40));
  (*(uint32_t*)rtguest::G(0x175e8a8)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_38));
  (*(uint64_t*)rtguest::G(0x175e8b0)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_30));
  (*(uint32_t*)rtguest::G(0x175e8b8)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_28));
  sceSifCallRpc(0x175e840,0x22,0,0x175e880,0x40,0,0,0);
  return 0;
}


RT_GUEST_FN(0x002586a0, guest::SndRpcCmd22)

}  // namespace guest
