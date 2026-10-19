// guest:0x00258308 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SndRpcPlayOrStop(uint32_t param_1, ...) {
    uint32_t uStack_28 = rtguest::allocFrame(8);
    uint32_t uStack_30 = rtguest::allocFrame(8);
    uint32_t uStack_38 = rtguest::allocFrame(8);
    uint32_t uStack_40 = rtguest::allocFrame(8);
    uint32_t uStack_48 = rtguest::allocFrame(8);
    uint32_t uStack_50 = rtguest::allocFrame(8);
    uint32_t uStack_58 = rtguest::allocFrame(8);
    uint32_t uStack_5c = rtguest::allocFrame(4);
  
  if ((param_1 & 0x8000ffff) == 0) {
    sceSifCallRpc(0x175e840,1,0,0,0,0,0,0);
  }
  else {
    (*(uint64_t*)rtguest::G(0x175e888)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_58));
    (*(uint64_t*)rtguest::G(0x175e890)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_50));
    (*(uint64_t*)rtguest::G(0x175e880)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_5c)),param_1);
    (*(uint64_t*)rtguest::G(0x175e898)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_48));
    (*(uint32_t*)rtguest::G(0x175e8a0)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_40));
    (*(uint32_t*)rtguest::G(0x175e8a8)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_38));
    (*(uint64_t*)rtguest::G(0x175e8b0)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_30));
    (*(uint32_t*)rtguest::G(0x175e8b8)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_28));
    sceSifCallRpc(0x175e840,0xd,0,0x175e880,0x40,0,0,0);
  }
  return 0;
}


RT_GUEST_FN(0x00258308, guest::SndRpcPlayOrStop)

}  // namespace guest
