// guest:0x0026abd0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ApplyAdjusterOffsets(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
    uint32_t uStack_20 = rtguest::allocFrame(4);
    uint32_t uStack_1c = rtguest::allocFrame(4);
    uint32_t uStack_18 = rtguest::allocFrame(4);
  ushort uVar1;
  int iVar2;
  int iVar3;
  
  param_1 = param_1 + 0xc;
  iVar3 = (int)param_2;
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + param_3))) == '\a') {
    iVar2 = param_3 * 4 + param_1;
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xb2))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xb2))) + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x32))) * 0x80;
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xb0))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xb0))) + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x33))) * 0x80;
  }
  uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_3 * 2 + param_1 + 0x20)));
  if ((uVar1 & 0x20) != 0) {
    (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x98))) = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x98))) + (*(float*)rtguest::G(0x333204));
  }
  if ((uVar1 & 0x40) != 0) {
    (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x98))) = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x98))) - (*(float*)rtguest::G(0x333208));
  }
  if ((uVar1 & 0x1000) != 0) {
    (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x94))) = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x94))) + 0.25;
  }
  if ((uVar1 & 0x4000) != 0) {
    (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x94))) = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x94))) - 0.25;
  }
  if ((uVar1 & 0x2000) != 0) {
    (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x90))) = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x90))) + 0.25;
  }
  if ((uVar1 & 0x8000) != 0) {
    (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x90))) = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x90))) - 0.25;
  }
  TransformRectangle(uStack_20,iVar3 + 0xb0);
  vu0UnitMatrix(param_2);
  vu0RotMatrixY((*(uint32_t*)rtguest::G((uint32_t)uStack_1c)),param_2,param_2);
  vu0RotMatrixX((*(uint32_t*)rtguest::G((uint32_t)uStack_20)),param_2,param_2);
  vu0RotMatrixZ((*(uint32_t*)rtguest::G((uint32_t)uStack_18)),param_2,param_2);
  vu0CopyVectorXYZ(iVar3 + 0x30,iVar3 + 0x90);
  return 0;
}


RT_GUEST_FN(0x0026abd0, guest::ApplyAdjusterOffsets)

}  // namespace guest
