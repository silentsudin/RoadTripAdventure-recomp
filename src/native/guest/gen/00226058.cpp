// guest:0x00226058 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ScaleCarLightMatrix(uint32_t param_1, uint32_t param_2, ...) {
uint32_t auStack_70 = rtguest::allocFrame(16);
uint32_t auStack_60 = rtguest::allocFrame(16);
uint32_t auStack_50 = rtguest::allocFrame(16);
uint32_t auStack_40 = rtguest::allocFrame(12);
    uint32_t uStack_34 = rtguest::allocFrame(4);
    uint32_t fStack_30 = rtguest::allocFrame(4);
    uint32_t fStack_2c = rtguest::allocFrame(4);
    uint32_t fStack_28 = rtguest::allocFrame(4);
  int iVar1;
  
  if (-2.0 < param_1) {
    param_1 = 0.0;
    (*(float*)rtguest::G((uint32_t)fStack_30)) = 1.0;
  }
  else {
    (*(float*)rtguest::G((uint32_t)fStack_30)) = param_1 * (*(float*)rtguest::G(0x332dcc)) + 0.5;
  }
  (*(float*)rtguest::G((uint32_t)fStack_28)) = param_1 * (*(uint32_t*)rtguest::G(0x332dd0)) + 1.0;
  if ((*(float*)rtguest::G((uint32_t)fStack_30)) < 0.0) {
    (*(float*)rtguest::G((uint32_t)fStack_30)) = 0.0;
  }
  if ((*(float*)rtguest::G((uint32_t)fStack_28)) < 0.25) {
    (*(float*)rtguest::G((uint32_t)fStack_28)) = 0.25;
  }
  (*(float*)rtguest::G((uint32_t)fStack_2c)) = (*(float*)rtguest::G((uint32_t)fStack_28));
  vu0MulVector(auStack_70,param_2,fStack_30);
  iVar1 = (int)param_2;
  vu0MulVector(auStack_60,iVar1 + 0x10,fStack_30);
  vu0MulVector(auStack_50,iVar1 + 0x20,fStack_30);
  vu0MulVector(auStack_40,iVar1 + 0x30,fStack_30);
  (*(uint32_t*)rtguest::G((uint32_t)uStack_34)) = 0x43000000;
  vifUploadLightMatrix(auStack_70);
  return 0;
}


RT_GUEST_FN(0x00226058, guest::ScaleCarLightMatrix)

}  // namespace guest
