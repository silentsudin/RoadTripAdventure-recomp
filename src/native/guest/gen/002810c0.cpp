// guest:0x002810c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _vsprintf_r(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(112);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x54) = (uint32_t)(int32_t)param_1;
    *(uint32_t*)rtguest::G(rt_sv+0x60) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x64) = 0x00000000u;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x7fffffffu;
    *(uint32_t*)rtguest::G(rt_sv+0x8) = 0x7fffffffu;
    *(uint32_t*)rtguest::G(rt_sv+0x10) = (uint32_t)(int32_t)param_2;
    *(uint32_t*)rtguest::G(rt_sv+0x0) = (uint32_t)(int32_t)param_2;
  }
uint32_t apuStack_70 = rtguest::allocFrame(8);
    uint32_t uStack_68 = rtguest::allocFrame(4);
    uint32_t uStack_64 = rtguest::allocFrame(2);
    rtguest::allocFrame(6);  // guest frame gap
    uint32_t uStack_5c = rtguest::allocFrame(4);
    rtguest::allocFrame(60);  // guest frame gap
    uint32_t uStack_1c = rtguest::allocFrame(4);
uint32_t puStack_60;
  
  (*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) = 0x7fffffff;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_68)) = 0x7fffffff;
  (*(uint16_t*)rtguest::G((uint32_t)uStack_64)) = 0x208;
  (*(uint32_t*)rtguest::G((uint32_t)apuStack_70 + (uint32_t)(0) * 4)) = param_2;
  puStack_60 = param_2;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_1c)) = param_1;
  RT_CALLRA(0x2810f8, guest::vfprintf(apuStack_70,param_3,param_4));
  (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)apuStack_70 + (uint32_t)(0) * 4)))) = 0;
  return 0;
}


RT_GUEST_FN(0x002810c0, guest::_vsprintf_r)

}  // namespace guest
