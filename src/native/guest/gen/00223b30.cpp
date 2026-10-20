// guest:0x00223b30 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawCarEffectB(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
  rtguest::FrameGuard rt_fg;
  ulong uVar1;
  long lVar2;
uint32_t auStack_e0 = rtguest::allocFrame(16);
uint32_t auStack_d0 = rtguest::allocFrame(16);
uint32_t auStack_c0 = rtguest::allocFrame(32);
uint32_t auStack_a0 = rtguest::allocFrame(16);
uint32_t auStack_90 = rtguest::allocFrame(16);
uint32_t auStack_80 = rtguest::allocFrame(16);
uint32_t auStack_70 = rtguest::allocFrame(16);
uint32_t auStack_60 = rtguest::allocFrame(16);
  
  vu0SubVector(auStack_60);
  vecNormalizeVu0(auStack_60,auStack_60);
  BuildTransformMatrix(auStack_e0,0x29d3a0,auStack_60);
  (*(uint32_t*)rtguest::G(0x29d37c)) = param_2;
  vifUploadColours(0x29d370,0x29d360);
  vu0UnitMatrix(auStack_a0);
  sceVu0ScaleVectorXYZ(rtguest::fbits(param_1),auStack_a0,auStack_e0);
  sceVu0ScaleVectorXYZ(rtguest::fbits(param_1),auStack_90,auStack_d0);
  sceVu0ScaleVectorXYZ(rtguest::fbits(param_1),auStack_80,auStack_c0);
  vu0CopyVectorXYZ(auStack_70,param_5);
  vifObjectMatrix(auStack_a0);
  if (param_6 == 2) {
    lVar2 = (long)(param_3 * 3 + 0x3f7c);
    uVar1 = (uint32_t)(param_3 * 3 + 0x3f7a);
  }
  else if (param_6 < 3) {
    if (param_6 != 1) goto LAB_00223d08;
    lVar2 = (long)(param_3 * 3 + 0x3f4c);
    uVar1 = (uint32_t)(param_3 * 3 + 0x3f4a);
  }
  else {
    if (param_6 != 3) {
      if (param_6 == 7) {
        vifBatchRegs(0x2007e5655540bf29,7,5,9);
      }
      goto LAB_00223d08;
    }
    lVar2 = (long)(param_3 * 3 + 0x3f6a);
    uVar1 = (uint32_t)(param_3 * 3 + 0x3f68);
  }
  vifBatchRegs(uVar1 | 0x15408000 | lVar2 << 0x25 | 0x2000000540000000,7,5,9);
LAB_00223d08:
  vifCallModel(0x290770);
  vu0CopyVectorXYZ(auStack_70,(int)param_5 + 0x10);
  vifObjectMatrix(auStack_a0);
  vifCallModel(0x290770);
  return 0;
}


RT_GUEST_FN(0x00223b30, guest::drawCarEffectB)

}  // namespace guest
