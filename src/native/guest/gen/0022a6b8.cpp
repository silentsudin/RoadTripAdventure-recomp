// guest:0x0022a6b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t UpdateAnimationPhaseMachine(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(112);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x60) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x64) = 0x00000000u;
  }
  char cVar1;
  long lVar2;
  uint uVar3;
  int iVar4;
uint32_t auStack_70 = rtguest::allocFrame(16);
uint32_t auStack_60 = rtguest::allocFrame(16);
uint32_t auStack_50 = rtguest::allocFrame(16);
  
  cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27)));
  if (cVar1 == '\0') {
    lVar2 = ApplyMotionClipFrame(0x17524b0,param_2,0,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))));
    if (lVar2 == 0) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) = 0xc;
      RT_CALLRA(0x22a70c, guest::PlayUiSound(0x35));
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27)));
    }
    else {
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27)));
    }
  }
  if (cVar1 == '\x06') {
    lVar2 = (long)(*(uint16_t*)rtguest::G((uint32_t)0x0175265c + 2));
    if ((long)(*(uint16_t*)rtguest::G((uint32_t)0x0175265c + 2)) < 0) {
      lVar2 = (long)((*(uint16_t*)rtguest::G((uint32_t)0x0175265c + 2)) + 0xff);
    }
    uVar3 = (uint32_t)lVar2 >> 8 & 0xff;
    if (uVar3 < 0x40) {
      uVar3 = uVar3 + 1;
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) + '\x01';
LAB_0022a784:
      uVar3 = uVar3 & 0xff;
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) = cVar1;
      (*(uint16_t*)rtguest::G((uint32_t)0x0175265c + 2)) = (short)(uVar3 << 8);
    }
    else {
      (*(uint16_t*)rtguest::G((uint32_t)0x0175265c + 2)) = (short)(uVar3 << 8);
      if (0x40 < uVar3) {
        uVar3 = uVar3 + 0xff;
        cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) + -1;
        goto LAB_0022a784;
      }
    }
    RT_CALLRA(0x22a79c, guest::vu0CopyVector(auStack_70,0x1752600));
    RT_CALLRA(0x22a7a4, guest::vu0UnitMatrix(0x17525d0));
    RT_CALLRA(0x22a7d4, guest::vu0RotMatrixY((float)(int)(*(uint16_t*)rtguest::G((uint32_t)0x0175265c + 2)) * 3.0517578e-05f * (*(float*)rtguest::G(0x332df4)),0x17525d0,0x17525d0))
    ;
    RT_CALLRA(0x22a7e0, guest::vu0CopyVectorXYZ(0x1752600,auStack_70));
    if (uVar3 == 0x40) {
      RT_CALLRA(0x22a7f4, guest::PlayUiSound(0x8035));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) = 7;
      cVar1 = '\a';
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) = 0;
    }
    else {
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27)));
    }
  }
  if (cVar1 == '\a') {
    ApplyMotionClipFrame(0x17525d0,(int)param_2 + 4,5,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))));
    lVar2 = ApplyMotionClipFrame(0x17524b0,param_2,2,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))));
    if (lVar2 == 0) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) = 0xc;
      RT_CALLRA(0x22a85c, guest::PlayUiSound(0x35));
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27)));
    }
    else {
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27)));
    }
  }
  if (((cVar1 == '\x02') || (cVar1 == '\t')) || (cVar1 == '\f')) {
    lVar2 = (long)(*(uint16_t*)rtguest::G((uint32_t)0x0175253c + 2));
    if ((long)(*(uint16_t*)rtguest::G((uint32_t)0x0175253c + 2)) < 0) {
      lVar2 = (long)((*(uint16_t*)rtguest::G((uint32_t)0x0175253c + 2)) + 0xff);
    }
    uVar3 = (uint32_t)lVar2 >> 8 & 0xff;
    if (uVar3 < 0x80) {
      uVar3 = uVar3 + 1;
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) + '\x01';
LAB_0022a8e4:
      uVar3 = uVar3 & 0xff;
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) = cVar1;
      (*(uint16_t*)rtguest::G((uint32_t)0x0175253c + 2)) = (short)(uVar3 << 8);
    }
    else {
      (*(uint16_t*)rtguest::G((uint32_t)0x0175253c + 2)) = (short)(uVar3 << 8);
      if (0x80 < uVar3) {
        uVar3 = uVar3 + 0xff;
        cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) + -1;
        goto LAB_0022a8e4;
      }
    }
    RT_CALLRA(0x22a8fc, guest::vu0CopyVector(auStack_60,0x17524e0));
    RT_CALLRA(0x22a904, guest::vu0UnitMatrix(0x17524b0));
    RT_CALLRA(0x22a934, guest::vu0RotMatrixY((float)(int)(*(uint16_t*)rtguest::G((uint32_t)0x0175253c + 2)) * 3.0517578e-05f * (*(float*)rtguest::G(0x332df8)),0x17524b0,0x17524b0))
    ;
    RT_CALLRA(0x22a940, guest::vu0CopyVectorXYZ(0x17524e0,auStack_60));
    if (uVar3 == 0x80) {
      RT_CALLRA(0x22a954, guest::PlayUiSound(0x8035));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) = 0;
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) + '\x01';
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) = cVar1;
    }
    else {
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27)));
    }
  }
  if (cVar1 == '\x03') {
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8)));
    if (0x1d < iVar4) {
      ApplyMotionClipFrame(0x17525d0,(int)param_2 + 4,4,iVar4 + -0x1e);
      iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8)));
    }
    lVar2 = ApplyMotionClipFrame(0x17524b0,param_2,1,iVar4);
    if (lVar2 == 0) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) = 4;
      RT_CALLRA(0x22a9d0, guest::PlayUiSound(0x35));
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27)));
    }
    else {
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27)));
    }
  }
  if (cVar1 != '\x04') goto LAB_0022aad4;
  lVar2 = (long)(*(uint16_t*)rtguest::G((uint32_t)0x0175265c + 2));
  if ((long)(*(uint16_t*)rtguest::G((uint32_t)0x0175265c + 2)) < 0) {
    lVar2 = (long)((*(uint16_t*)rtguest::G((uint32_t)0x0175265c + 2)) + 0xff);
  }
  uVar3 = (uint32_t)lVar2 >> 8 & 0xff;
  if (uVar3 < 0x80) {
    uVar3 = uVar3 + 1;
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) + '\x01';
LAB_0022aa4c:
    uVar3 = uVar3 & 0xff;
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) = cVar1;
    (*(uint16_t*)rtguest::G((uint32_t)0x0175265c + 2)) = (short)(uVar3 << 8);
  }
  else {
    (*(uint16_t*)rtguest::G((uint32_t)0x0175265c + 2)) = (short)(uVar3 << 8);
    if (0x80 < uVar3) {
      uVar3 = uVar3 + 0xff;
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) + -1;
      goto LAB_0022aa4c;
    }
  }
  RT_CALLRA(0x22aa64, guest::vu0CopyVector(auStack_50,0x1752600));
  RT_CALLRA(0x22aa6c, guest::vu0UnitMatrix(0x17525d0));
  RT_CALLRA(0x22aa9c, guest::vu0RotMatrixY((float)(int)(*(uint16_t*)rtguest::G((uint32_t)0x0175265c + 2)) * 3.0517578e-05f * (*(float*)rtguest::G(0x332dfc)),0x17525d0,0x17525d0));
  RT_CALLRA(0x22aaa8, guest::vu0CopyVectorXYZ(0x1752600,auStack_50));
  if (uVar3 == 0x80) {
    RT_CALLRA(0x22aabc, guest::PlayUiSound(0x8035));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) = 5;
    cVar1 = '\x05';
  }
  else {
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27)));
  }
LAB_0022aad4:
  if (cVar1 == '\n') {
    lVar2 = ApplyMotionClipFrame(0x17524b0,param_2,3,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))));
    if (lVar2 == 0) {
      cVar1 = '\v';
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) = 0xb;
    }
    else {
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27)));
    }
  }
  if (cVar1 == '\x0e') {
    lVar2 = ApplyMotionClipFrame(0x17524b0,param_2,6,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))));
    if (lVar2 == 0) {
      cVar1 = '\x0f';
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) = 0xf;
    }
    else {
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27)));
    }
  }
  if ((cVar1 == '\x10') &&
     (lVar2 = ApplyMotionClipFrame(0x17524b0,param_2,7,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8)))), lVar2 == 0)) {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) = 0xb;
  }
  return 0;
}


RT_GUEST_FN(0x0022a6b8, guest::UpdateAnimationPhaseMachine)

}  // namespace guest
