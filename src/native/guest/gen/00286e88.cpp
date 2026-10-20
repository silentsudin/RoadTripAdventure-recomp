// guest:0x00286e88 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t printf__00286e88(uint64_t param_1, uint64_t param_2, uint64_t param_3, uint64_t param_4, uint64_t param_5, uint64_t param_6, uint64_t param_7, uint64_t param_8, ...) {
  rtguest::FrameGuard rt_fg(144);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x58) = (uint32_t)(int32_t)param_2;
    *(uint32_t*)rtguest::G(rt_sv+0x5c) = (uint32_t)((int64_t)(int32_t)param_2 >> 32);
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
    *(uint32_t*)rtguest::G(rt_sv+0x60) = (uint32_t)(int32_t)param_3;
    *(uint32_t*)rtguest::G(rt_sv+0x64) = (uint32_t)((int64_t)(int32_t)param_3 >> 32);
    *(uint32_t*)rtguest::G(rt_sv+0x68) = (uint32_t)(int32_t)param_4;
    *(uint32_t*)rtguest::G(rt_sv+0x6c) = (uint32_t)((int64_t)(int32_t)param_4 >> 32);
    *(uint32_t*)rtguest::G(rt_sv+0x70) = (uint32_t)(int32_t)param_5;
    *(uint32_t*)rtguest::G(rt_sv+0x74) = (uint32_t)((int64_t)(int32_t)param_5 >> 32);
    *(uint32_t*)rtguest::G(rt_sv+0x78) = (uint32_t)(int32_t)param_6;
    *(uint32_t*)rtguest::G(rt_sv+0x7c) = (uint32_t)((int64_t)(int32_t)param_6 >> 32);
  }
    // spill area must be one contiguous block: _printf walks it 8 bytes per arg
    rtguest::allocFrame(88);  // guest frame gap (sp+0x0..0x57)
    uint32_t uStack_38 = rtguest::allocFrame(56);  // sp+0x58 (Ghidra entry-sp -0x38, frame 0x90)
  
  (*(uint64_t*)rtguest::G((uint32_t)uStack_38)) = param_2;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_38 + (uint32_t)(1) * 8)) = param_3;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_38 + (uint32_t)(2) * 8)) = param_4;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_38 + (uint32_t)(3) * 8)) = param_5;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_38 + (uint32_t)(4) * 8)) = param_6;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_38 + (uint32_t)(5) * 8)) = param_7;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_38 + (uint32_t)(6) * 8)) = param_8;
  RT_CALLRA(0x286eac, guest::_printf(param_1,uStack_38));
  return 0;
}


RT_GUEST_FN(0x00286e88, guest::printf__00286e88)

}  // namespace guest
