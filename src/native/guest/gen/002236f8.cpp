// guest:0x002236f8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawCarShowroomRig(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(96);
  undefined8 uVar1;
uint32_t auStack_60 = rtguest::allocFrame(64);
  
  uVar1 = GetSceneCamLookatIndex(1);
  vu0DropShadowMatrix(0,0xbf800000,0,auStack_60,uVar1,0);
  drawCarShowroom(auStack_60,param_1,param_2);
  return 0;
}


RT_GUEST_FN(0x002236f8, guest::DrawCarShowroomRig)

}  // namespace guest
