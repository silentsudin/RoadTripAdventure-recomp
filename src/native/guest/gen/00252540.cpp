// guest:0x00252540 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t NormalizeQuatAndRotate(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
uint32_t auStack_40 = rtguest::allocFrame(16);
    uint32_t uStack_30 = rtguest::allocFrame(8);
    uint32_t uStack_28 = rtguest::allocFrame(8);
  float fVar1;
  
  sceVu0ScaleVectorXYZ(rtguest::fbits(0xbf800000),auStack_40,param_3);
  if (param_2 == 0) {
    (*(uint64_t*)rtguest::G((uint32_t)uStack_30)) = 0x3f80000000000000;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_28)) = 0;
  }
  else {
    (*(uint64_t*)rtguest::G((uint32_t)uStack_30)) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)param_2));
    (*(uint64_t*)rtguest::G((uint32_t)uStack_28)) = (*(uint64_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * 8));
  }
  vecNormalizeVu0(uStack_30,uStack_30);
  fVar1 = rtguest::fbits(vecDotProductVu0(uStack_30,auStack_40));
  sceVu0ScaleVectorXYZ(fVar1 + fVar1,param_1,uStack_30);
  vu0SubVector(param_1,param_1,auStack_40);
  return 0;
}


RT_GUEST_FN(0x00252540, guest::NormalizeQuatAndRotate)

}  // namespace guest
