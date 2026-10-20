// guest:0x0021f098 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t BuildMenuSceneCamera(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(96);
  int iVar1;
uint32_t auStack_60 = rtguest::allocFrame(48);
uint32_t auStack_30 = rtguest::allocFrame(16);
  
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x23))) == '\x16') {
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8)));
    if (iVar1 == 0) {
      (*(float*)rtguest::G(0x29ccd0)) = (*(float*)rtguest::G(0x332c54));
    }
    if (500.0 < (*(float*)rtguest::G(0x29ccd0))) {
      (*(float*)rtguest::G(0x29ccd0)) = (*(float*)rtguest::G(0x29ccd0)) / (*(float*)rtguest::G(0x332c58));
    }
    if (iVar1 < 0x101) {
      (*(uint32_t*)rtguest::G(0x29ccd4)) = (short)iVar1 * 5 + 0x380;
    }
    buildCamera(0x177e6d0,0x29ccc0,0,0);
  }
  else {
    vu0UnitMatrix(auStack_60);
    vu0CopyVector(auStack_30,0x177ace0);
    buildCamera(0x177e6d0,0x29ca00u + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x23))) * 8,auStack_60,auStack_60);
  }
  return 0;
}


RT_GUEST_FN(0x0021f098, guest::BuildMenuSceneCamera)

}  // namespace guest
