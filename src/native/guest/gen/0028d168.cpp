// guest:0x0028d168 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceDeci2ExReqSend(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
    uint32_t uStack_20 = rtguest::allocFrame(4);
    uint32_t iStack_1c = rtguest::allocFrame(4);
  
  (*(int32_t*)rtguest::G((uint32_t)iStack_1c)) = (int)param_2;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_20)) = param_1;
  Deci2Call(0xfffffffffffffff9,uStack_20);
  return 0;
}


RT_GUEST_FN(0x0028d168, guest::sceDeci2ExReqSend)

}  // namespace guest
