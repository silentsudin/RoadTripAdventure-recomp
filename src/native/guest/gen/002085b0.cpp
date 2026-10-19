// guest:0x002085b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ReflectVectorVU0(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  float fVar1;
  float fVar2;
uint32_t auStack_50 = rtguest::allocFrame(16);
uint32_t auStack_40 = rtguest::allocFrame(16);
  
  vecNormalizeVu0(auStack_40);
  fVar1 = (float)vecDotProductVu0(param_3,param_3);
  fVar1 = SQRT(fVar1);
  if (fVar1 == 0.0) {
    vu0CopyVector(param_1,param_3);
  }
  else {
    vecDivByScalarVu0(-fVar1,auStack_50,param_3);
    fVar2 = (float)vecDotProductVu0(auStack_50,param_2);
    sceVu0ScaleVectorXYZ(fVar2 + fVar2,param_1,auStack_40);
    vu0SubVector(param_1,param_1,auStack_50);
    vu0ScaleVector(fVar1,param_1,param_1);
  }
  return 0;
}


RT_GUEST_FN(0x002085b0, guest::ReflectVectorVU0)

}  // namespace guest
