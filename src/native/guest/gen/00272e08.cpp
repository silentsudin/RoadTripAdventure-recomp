// guest:0x00272e08 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t vu0NormalLightMatrix(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  int iVar1;
uint32_t auStack_60 = rtguest::allocFrame(16);
  
  vu0ScaleVector(rtguest::fbits(0xbf800000),auStack_60);
  vecNormalizeVu0(param_1,auStack_60);
  vu0ScaleVector(rtguest::fbits(0xbf800000),auStack_60,param_3);
  iVar1 = (int)param_1;
  vecNormalizeVu0(iVar1 + 0x10,auStack_60);
  vu0ScaleVector(rtguest::fbits(0xbf800000),auStack_60,param_4);
  vecNormalizeVu0(iVar1 + 0x20,auStack_60);
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x30))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x3c))) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x38))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x34))) = 0;
  mat4Transpose(param_1,param_1);
  return 0;
}


RT_GUEST_FN(0x00272e08, guest::vu0NormalLightMatrix)

}  // namespace guest
