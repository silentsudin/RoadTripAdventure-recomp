// guest:0x002236f8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t DrawCarShowroomRig(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x50) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x54) = 0x00000000u;
  }
  undefined8 uVar1;
uint32_t auStack_60 = rtguest::allocFrame(64);
  
  uVar1 = RT_CALLRA(0x223710, guest::GetSceneCamLookatIndex(1));
  RT_CALLRA(0x223730, guest::vu0DropShadowMatrix(0,0xbf800000,0,auStack_60,uVar1,0));
  RT_CALLRA(0x223740, guest::drawCarShowroom(auStack_60,param_1,param_2));
  return 0;
}


RT_GUEST_FN(0x002236f8, guest::DrawCarShowroomRig)

}  // namespace guest
