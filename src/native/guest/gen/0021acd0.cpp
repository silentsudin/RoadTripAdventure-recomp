// guest:0x0021acd0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateSuspensionFlex(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  
  fVar1 = (*(uint32_t*)rtguest::G(0x332be0));
  fVar4 = (*(uint32_t*)rtguest::G(0x332bdc));
  fVar3 = (*(uint32_t*)rtguest::G(0x332bb8));
  fVar2 = (*(uint32_t*)rtguest::G(0x332bb4));
  if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x184))) + 8))) & 0x400) == 0) {
    fVar3 = ((float)(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1b8))) * 3.0517578e-05) / (*(uint32_t*)rtguest::G(0x332bc0));
    fVar2 = (*(uint32_t*)rtguest::G(0x332bc4));
    if ((fVar3 <= (*(uint32_t*)rtguest::G(0x332bc4))) && (fVar2 = fVar3, fVar3 < (*(uint32_t*)rtguest::G(0x332bc8)))) {
      fVar2 = (*(uint32_t*)rtguest::G(0x332bc8));
    }
    fVar2 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x80))) + fVar2;
    if (fVar2 < (*(uint32_t*)rtguest::G(0x332bcc))) {
      fVar2 = fVar2 + (*(uint32_t*)rtguest::G(0x332bd0));
    }
    else if ((*(uint32_t*)rtguest::G(0x332bd4)) < fVar2) {
      fVar2 = fVar2 - (*(uint32_t*)rtguest::G(0x332bd8));
    }
    (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x80))) = fVar2;
    fVar2 = ((float)(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1bc))) * 3.0517578e-05) / fVar4;
    fVar5 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x88)));
    fVar4 = (*(uint32_t*)rtguest::G(0x332be4));
    fVar3 = fVar1;
    if (fVar2 <= fVar1) goto LAB_0021aea4;
  }
  else {
    fVar1 = ((float)(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1b8))) * 3.0517578e-05) / (*(float*)rtguest::G(0x332b98));
    fVar4 = (*(uint32_t*)rtguest::G(0x332b9c));
    if ((fVar1 <= (*(uint32_t*)rtguest::G(0x332b9c))) && (fVar4 = fVar1, fVar1 < (*(uint32_t*)rtguest::G(0x332ba0)))) {
      fVar4 = (*(uint32_t*)rtguest::G(0x332ba0));
    }
    fVar4 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x80))) + fVar4;
    if (fVar4 < (*(uint32_t*)rtguest::G(0x332ba4))) {
      fVar4 = fVar4 + (*(uint32_t*)rtguest::G(0x332ba8));
    }
    else if ((*(uint32_t*)rtguest::G(0x332bac)) < fVar4) {
      fVar4 = fVar4 - (*(uint32_t*)rtguest::G(0x332bb0));
    }
    (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x80))) = fVar4;
    fVar2 = ((float)(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1bc))) * 3.0517578e-05) / fVar2;
    fVar5 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x88)));
    fVar4 = (*(uint32_t*)rtguest::G(0x332bbc));
    if (fVar2 <= fVar3) {
LAB_0021aea4:
      fVar3 = fVar2;
      if (fVar2 < fVar4) {
        fVar3 = fVar4;
      }
    }
  }
  fVar5 = fVar5 + fVar3;
  if (fVar5 < (*(uint32_t*)rtguest::G(0x332be8))) {
    fVar5 = fVar5 + (*(uint32_t*)rtguest::G(0x332bec));
  }
  else {
    if (fVar5 <= (*(uint32_t*)rtguest::G(0x332bf0))) {
      (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x88))) = fVar5;
      goto LAB_0021aef8;
    }
    fVar5 = fVar5 - (*(uint32_t*)rtguest::G(0x332bf4));
  }
  (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x88))) = fVar5;
LAB_0021aef8:
  (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x84))) =
       (float)((int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1ce))) << 7) * (*(uint32_t*)rtguest::G(0x332bf8)) * 3.0517578e-05;
  return 0;
}


RT_GUEST_FN(0x0021acd0, guest::UpdateSuspensionFlex)

}  // namespace guest
