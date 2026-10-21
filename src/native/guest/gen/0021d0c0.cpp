// guest:0x0021d0c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t TestSegmentInBoxWindow(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(32);
    uint32_t fStack_20 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t fStack_18 = rtguest::allocFrame(4);
    rtguest::allocFrame(12);  // guest frame gap
    uint32_t fStack_8 = rtguest::allocFrame(4);
  undefined4 uVar1;
  float fVar2;
  float fVar3;
  
  (*(float*)rtguest::G((uint32_t)fStack_20)) = (*(float*)rtguest::G((uint32_t)param_3));
  (*(float*)rtguest::G((uint32_t)fStack_8)) = (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(2) * sizeof(float)));
  (*(float*)rtguest::G((uint32_t)fStack_18)) = (*(float*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(float)));
  fVar2 = (*(float*)rtguest::G((uint32_t)param_4));
  if (((param_1 - 110.0 <= (*(float*)rtguest::G((uint32_t)fStack_20))) || (uVar1 = 0, param_1 - 110.0 <= fVar2)) &&
     (((*(float*)rtguest::G((uint32_t)fStack_20)) <= param_1 + 110.0 || (uVar1 = 0, fVar2 <= param_1 + 110.0)))) {
    fVar3 = param_1 - 110.0;
    if ((*(float*)rtguest::G((uint32_t)fStack_20)) < fVar3) {
      (*(float*)rtguest::G((uint32_t)fStack_18)) = rtguest::fdiv((((*(float*)rtguest::G((uint32_t)fStack_8)) - (*(float*)rtguest::G((uint32_t)fStack_18))) * (fVar3 - (*(float*)rtguest::G((uint32_t)fStack_20)))), (fVar2 - (*(float*)rtguest::G((uint32_t)fStack_20)))) + (*(float*)rtguest::G((uint32_t)fStack_18));
      (*(float*)rtguest::G((uint32_t)fStack_20)) = fVar3;
    }
    if (fVar2 < fVar3) {
      (*(float*)rtguest::G((uint32_t)fStack_8)) = rtguest::fdiv((((*(float*)rtguest::G((uint32_t)fStack_18)) - (*(float*)rtguest::G((uint32_t)fStack_8))) * (fVar3 - fVar2)), ((*(float*)rtguest::G((uint32_t)fStack_20)) - fVar2)) + (*(float*)rtguest::G((uint32_t)fStack_8));
      fVar2 = fVar3;
    }
    param_1 = param_1 + 110.0;
    if (param_1 < (*(float*)rtguest::G((uint32_t)fStack_20))) {
      (*(float*)rtguest::G((uint32_t)fStack_18)) = rtguest::fdiv((((*(float*)rtguest::G((uint32_t)fStack_8)) - (*(float*)rtguest::G((uint32_t)fStack_18))) * (param_1 - (*(float*)rtguest::G((uint32_t)fStack_20)))), (fVar2 - (*(float*)rtguest::G((uint32_t)fStack_20)))) + (*(float*)rtguest::G((uint32_t)fStack_18))
      ;
      (*(float*)rtguest::G((uint32_t)fStack_20)) = param_1;
    }
    if (param_1 < fVar2) {
      (*(float*)rtguest::G((uint32_t)fStack_8)) = rtguest::fdiv((((*(float*)rtguest::G((uint32_t)fStack_18)) - (*(float*)rtguest::G((uint32_t)fStack_8))) * (param_1 - fVar2)), ((*(float*)rtguest::G((uint32_t)fStack_20)) - fVar2)) + (*(float*)rtguest::G((uint32_t)fStack_8));
    }
    if ((((-110.0 <= (*(float*)rtguest::G((uint32_t)fStack_18)) - param_2) || (uVar1 = 0, -110.0 <= (*(float*)rtguest::G((uint32_t)fStack_8)) - param_2)) &&
        (uVar1 = 1, 110.0 < (*(float*)rtguest::G((uint32_t)fStack_18)) - param_2)) && (uVar1 = 0, (*(float*)rtguest::G((uint32_t)fStack_8)) - param_2 <= 110.0)) {
      uVar1 = 1;
    }
  }
  return uVar1;
}


RT_GUEST_FN(0x0021d0c0, guest::TestSegmentInBoxWindow)

}  // namespace guest
