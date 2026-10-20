// guest:0x00258218 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SndRpcCmdCArgs(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, uint32_t param_7, uint32_t param_8, uint32_t param_9, uint32_t param_10, ...) {
  rtguest::FrameGuard rt_fg(96);
    uint32_t uStack_44 = rtguest::allocFrame(4);
    uint32_t uStack_40 = rtguest::allocFrame(8);
  
  (*(uint64_t*)rtguest::G(0x175e880)) = CONCAT44(param_1,0x3f1);
  (*(uint64_t*)rtguest::G(0x175e888)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_44)),param_2);
  (*(uint64_t*)rtguest::G(0x175e890)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_40));
  (*(uint64_t*)rtguest::G(0x175e898)) = CONCAT44(param_4,param_3);
  (*(uint64_t*)rtguest::G(0x175e8a0)) = CONCAT44(param_6,param_5);
  (*(uint64_t*)rtguest::G(0x175e8a8)) = CONCAT44(param_8,param_7);
  (*(uint64_t*)rtguest::G(0x175e8b0)) = CONCAT44(param_10,param_9);
  RT_CALLRA(0x2582f4, guest::sceSifCallRpc(0x175e840,0xc,0,0x175e880,0x40,0,0,0));
  return 0;
}


RT_GUEST_FN(0x00258218, guest::SndRpcCmdCArgs)

}  // namespace guest
