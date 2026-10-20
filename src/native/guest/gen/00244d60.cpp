// guest:0x00244d60 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawCarouselCells(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
  }
uint32_t afStack_40 = rtguest::allocFrame(8);
    uint32_t fStack_38 = rtguest::allocFrame(4);
  bool bVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  
  iVar3 = param_1 + param_2 * 0xd0;
  RT_CALLRA(0x244da0, guest::vifDirect(param_1 + 0x60,2));
  RT_CALLRA(0x244dac, guest::vifDirect(iVar3 + 0x80,0xd));
  RT_CALLRA(0x244db8, guest::vifDirect(param_1 + 0xd80,2));
  RT_CALLRA(0x244dc4, guest::vifDirect(iVar3 + 0xda0,0xd));
  if (param_2 < 10) {
    iVar3 = param_2 * 0x10;
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 10))) == '\x01') {
      uVar4 = (uint32_t)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x2be158 + (uint32_t)(iVar3) * 4));
      bVar1 = uVar4 < 0x12;
    }
    else {
      uVar4 = (uint32_t)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x2be1d8 + (uint32_t)(iVar3) * 4));
      bVar1 = uVar4 < 0x12;
    }
  }
  else {
    if (param_2 == 0xc) {
      uVar4 = 0x19;
      iVar3 = 0xc0;
    }
    else if (param_2 == 0xd) {
      uVar4 = 0x1b;
      iVar3 = 0xd0;
    }
    else {
      uVar4 = param_2 + 0x10;
      iVar3 = param_2 << 4;
    }
    bVar1 = (int)uVar4 < 0x12;
  }
  if (bVar1) {
    param_1 = (iVar3 - param_2) * 0x20 + param_1;
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(afStack_40)), 0, 0x10));
    RT_CALLRA(0x244e78, guest::vifUploadSceneParams(param_1 + 0x1f60));
    RT_CALLRA(0x244e80, guest::vifUploadCamera(param_1 + 0x1fb0));
    RT_CALLRA(0x244e88, guest::vifCameraSetup());
    uVar2 = uVar4;
    if ((int)uVar4 < 0) {
      uVar2 = uVar4 + 7;
    }
    (*(float*)rtguest::G((uint32_t)afStack_40 + (uint32_t)(0) * 4)) = (float)(uVar4 & 7) * -200.0;
    (*(float*)rtguest::G((uint32_t)fStack_38)) = (float)((int)uVar2 >> 3) * -200.0;
    RT_CALLRA(0x244edc, guest::vifDirect(param_1 + 0x1f90,2));
    RT_CALLRA(0x244ee4, guest::vifSetBatchOffset(afStack_40));
    RT_CALLRA(0x244efc, guest::vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(uVar4 * 4 + (*(int32_t*)rtguest::G(0x177ab24)))))));
  }
  else {
    RT_CALLRA(0x244f20, guest::vifDirect(param_1 + uVar4 * 0xf0 + 0x2c80,2));
    RT_CALLRA(0x244f38, guest::vifDirect(param_1 + (iVar3 - param_2) * 0x10 + 0x3d80,0xd));
  }
  return 0;
}


RT_GUEST_FN(0x00244d60, guest::drawCarouselCells)

}  // namespace guest
