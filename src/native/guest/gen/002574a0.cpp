// guest:0x002574a0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SoundRpcPlaySequence(uint32_t param_1, ...) {
    uint32_t uStack_ac = rtguest::allocFrame(4);
    rtguest::allocFrame(76);  // guest frame gap
    uint32_t uStack_5c = rtguest::allocFrame(4);
    uint32_t uStack_58 = rtguest::allocFrame(8);
    uint32_t uStack_50 = rtguest::allocFrame(8);
    uint32_t uStack_48 = rtguest::allocFrame(8);
    uint32_t uStack_40 = rtguest::allocFrame(8);
    uint32_t uStack_38 = rtguest::allocFrame(8);
  
  (*(uint64_t*)rtguest::G(0x175e880)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_ac)),param_1);
  sceSifCallRpc(0x175e840,0x23,0,0x175e880,0x40,0,0,0);
  (*(uint64_t*)rtguest::G(0x175e880)) = (uint32_t)param_1;
  (*(uint64_t*)rtguest::G(0x175e888)) = 1;
  (*(uint64_t*)rtguest::G(0x175e890)) = (uint64_t)(*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) << 0x20;
  (*(uint64_t*)rtguest::G(0x175e898)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_58));
  (*(uint32_t*)rtguest::G(0x175e8a0)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_50));
  (*(uint32_t*)rtguest::G(0x175e8a8)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_48));
  (*(uint64_t*)rtguest::G(0x175e8b0)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_40));
  (*(uint32_t*)rtguest::G(0x175e8b8)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_38));
  sceSifCallRpc(0x175e840,0x22,0,0x175e880,0x40,0,0,0);
  return 0;
}


RT_GUEST_FN(0x002574a0, guest::SoundRpcPlaySequence)

}  // namespace guest
