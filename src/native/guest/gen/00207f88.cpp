// guest:0x00207f88 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t BuildTransformMatrix(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  
  iVar1 = (int)param_1 + 0x10;
  vu0UnitMatrix();
  vecCrossProductVu0(param_1,param_2,param_3);
  vecNormalizeVu0(param_1,param_1);
  vu0CopyVector(iVar1,param_2);
  vecCrossProductVu0((int)param_1 + 0x20,param_1,iVar1);
  return 0;
}


RT_GUEST_FN(0x00207f88, guest::BuildTransformMatrix)

}  // namespace guest
