// guest:0x0028d030 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceDeci2Open(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(32);
    uint32_t uStack_20 = rtguest::allocFrame(4);
    uint32_t uStack_1c = rtguest::allocFrame(4);
    uint32_t uStack_18 = rtguest::allocFrame(4);
uint32_t puStack_14;
  
  (*(uint32_t*)rtguest::G((uint32_t)uStack_20)) = param_1 & 0xffff;
  puStack_14 = 0x2177a0a8;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_1c)) = param_2;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_18)) = param_3;
  Deci2Call(1,uStack_20);
  return 0;
}


RT_GUEST_FN(0x0028d030, guest::sceDeci2Open)

}  // namespace guest
