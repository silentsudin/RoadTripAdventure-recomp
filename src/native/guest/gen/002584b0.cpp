// guest:0x002584b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GsSetupFrameContext(uint32_t param_1, uint64_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x28, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x48, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x38, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x40, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x50) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x54) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_3);  // walk: = param_3
    rtguest::setS32(2, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(3, (uint32_t)param_5);  // walk: = param_5
    rtguest::setS32(5, (uint32_t)param_4);  // walk: = param_4
  }
    uint32_t iStack_60 = rtguest::allocFrame(4);
    uint32_t uStack_5c = rtguest::allocFrame(4);
    uint32_t uStack_58 = rtguest::allocFrame(2);
    uint32_t uStack_56 = rtguest::allocFrame(2);
    uint32_t uStack_54 = rtguest::allocFrame(4);
    uint32_t uStack_50 = rtguest::allocFrame(4);
  uint uVar1;
  
  uVar1 = param_1 % 2;
  RT_CALLRA(0x258504, guest::sceSdRemote(1,0x8050,uVar1 | 0x1d00,param_2));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) = param_3 | 0x100;
  (*(int32_t*)rtguest::G((uint32_t)iStack_60)) = param_1;
  (*(uint16_t*)rtguest::G((uint32_t)uStack_58)) = param_4;
  (*(uint16_t*)rtguest::G((uint32_t)uStack_56)) = param_4;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_54)) = param_5;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_50)) = param_6;
  RT_CALLRA(0x258534, guest::sceSdRemote(1,0x8130,uVar1,iStack_60));
  RT_CALLRA(0x258548, guest::sceSdRemote(1,0x8070,uVar1 | 2,1));
  return 0;
}


RT_GUEST_FN(0x002584b0, guest::GsSetupFrameContext)

}  // namespace guest
