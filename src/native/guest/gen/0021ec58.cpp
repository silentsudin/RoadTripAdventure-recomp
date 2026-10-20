// guest:0x0021ec58 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RunCameraZoomTransition(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(176);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0xa0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xa4) = 0x00000000u;
  }
  int iVar1;
  uint uVar2;
  int iVar3;
uint32_t auStack_b0 = rtguest::allocFrame(48);
uint32_t auStack_80 = rtguest::allocFrame(16);
uint32_t auStack_70 = rtguest::allocFrame(64);
  
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8)));
  uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) & 1;
  iVar3 = uVar2 * 0x20;
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea0a + iVar3))) = 0;
  if (iVar1 < 0x40) {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea0c + iVar3))) == 0) {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea06 + iVar3))) = 0x8000;
    }
  }
  else if (iVar1 < 0x80) {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea0c + iVar3))) == 0) {
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea06 + iVar3))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea06 + iVar3))) + 0x200;
    }
  }
  else {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0xfffffbff;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xc))) = 0x21e690;
  }
  RT_CALLRA(0x21ed4c, guest::ComputeChaseCameraBasis(auStack_b0,auStack_70,0x0177e9f0 + iVar3,0x177ac50 + uVar2 * 0x4e));
  RT_CALLRA(0x21ed58, guest::FixedPosToFloatVec4(auStack_80,0x0177acf0 + uVar2 * 0x9c));
  RT_CALLRA(0x21ed6c, guest::buildCamera(0x177e6d0 + uVar2 * 400,0x0177e9f0 + iVar3,auStack_b0,auStack_70));
  return 0;
}


RT_GUEST_FN(0x0021ec58, guest::RunCameraZoomTransition)

}  // namespace guest
