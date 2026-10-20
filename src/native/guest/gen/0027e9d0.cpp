// guest:0x0027e9d0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _printf_r(uint32_t param_1, uint32_t param_2, uint64_t param_3, uint64_t param_4, uint64_t param_5, uint64_t param_6, uint64_t param_7, uint64_t param_8, ...) {
  rtguest::FrameGuard rt_fg;
    uint32_t uStack_30 = rtguest::allocFrame(8);
    uint32_t uStack_28 = rtguest::allocFrame(8);
    uint32_t uStack_20 = rtguest::allocFrame(8);
    uint32_t uStack_18 = rtguest::allocFrame(8);
    uint32_t uStack_10 = rtguest::allocFrame(8);
    uint32_t uStack_8 = rtguest::allocFrame(8);
  
  (*(uint64_t*)rtguest::G((uint32_t)uStack_30)) = param_3;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_28)) = param_4;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_20)) = param_5;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_18)) = param_6;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_10)) = param_7;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_8)) = param_8;
  _vfprintf_r(param_1,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))),param_2,uStack_30);
  return 0;
}


RT_GUEST_FN(0x0027e9d0, guest::_printf_r)

}  // namespace guest
