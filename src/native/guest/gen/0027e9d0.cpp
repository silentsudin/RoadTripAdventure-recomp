// guest:0x0027e9d0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _printf_r(uint32_t param_1, uint32_t param_2, uint64_t param_3, uint64_t param_4, uint64_t param_5, uint64_t param_6, uint64_t param_7, uint64_t param_8, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
    *(uint32_t*)rtguest::G(rt_sv+0x30) = (uint32_t)(int32_t)param_3;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = (uint32_t)((int64_t)(int32_t)param_3 >> 32);
    *(uint32_t*)rtguest::G(rt_sv+0x38) = (uint32_t)(int32_t)param_4;
    *(uint32_t*)rtguest::G(rt_sv+0x3c) = (uint32_t)((int64_t)(int32_t)param_4 >> 32);
    *(uint32_t*)rtguest::G(rt_sv+0x40) = (uint32_t)(int32_t)param_5;
    *(uint32_t*)rtguest::G(rt_sv+0x44) = (uint32_t)((int64_t)(int32_t)param_5 >> 32);
    *(uint32_t*)rtguest::G(rt_sv+0x48) = (uint32_t)(int32_t)param_6;
    *(uint32_t*)rtguest::G(rt_sv+0x4c) = (uint32_t)((int64_t)(int32_t)param_6 >> 32);
  }
    rtguest::allocFrame(48);  // guest frame gap (sp+0x0..0x2f)
    uint32_t uStack_30 = rtguest::allocFrame(8);  // sp+0x30 (Ghidra entry-sp -0x30, frame 0x60)
    uint32_t uStack_28 = rtguest::allocFrame(8);  // sp+0x38 (Ghidra entry-sp -0x28, frame 0x60)
    uint32_t uStack_20 = rtguest::allocFrame(8);  // sp+0x40 (Ghidra entry-sp -0x20, frame 0x60)
    uint32_t uStack_18 = rtguest::allocFrame(8);  // sp+0x48 (Ghidra entry-sp -0x18, frame 0x60)
    uint32_t uStack_10 = rtguest::allocFrame(8);  // sp+0x50 (Ghidra entry-sp -0x10, frame 0x60)
    uint32_t uStack_8 = rtguest::allocFrame(8);  // sp+0x58 (Ghidra entry-sp -0x8, frame 0x60)
  
  (*(uint64_t*)rtguest::G((uint32_t)uStack_30)) = param_3;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_28)) = param_4;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_20)) = param_5;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_18)) = param_6;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_10)) = param_7;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_8)) = param_8;
  RT_CALLRA(0x27ea18, guest::_vfprintf_r(param_1,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))),param_2,uStack_30));
  return 0;
}


RT_GUEST_FN(0x0027e9d0, guest::_printf_r)

}  // namespace guest
