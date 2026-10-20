// guest:0x002213e8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawCarWheelsMid(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  ulong uVar2;
  int iVar3;
uint32_t piVar4;
uint32_t puVar5;
uint32_t auStack_d0 = rtguest::allocFrame(64);
uint32_t auStack_90 = rtguest::allocFrame(48);
uint32_t auStack_60 = rtguest::allocFrame(16);
  
  iVar3 = 0;
  uVar2 = (uint32_t)(int)((uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 6))) * 2 + 0x1c44);
  vifFixedRegs(uVar2 | 0x20138c85dd308000,7,
               uVar2 | 0x1d408000 | (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 5))) << 0x38 | 0x138c85c0000000,7);
  vu0UnitMatrix(auStack_90);
  vu0RotMatrixX((*(uint32_t*)rtguest::G((uint32_t)param_2)),auStack_90,auStack_90);
  vu0RotMatrixY((*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(uint32_t))),auStack_90,auStack_90);
  piVar4 = 0x0177eaf8;
  puVar5 = 0x0029caf0;
  do {
    if (iVar3 == 2) {
      vu0UnitMatrix(auStack_90);
      vu0RotMatrixX((*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(uint32_t))),auStack_90,auStack_90);
    }
    iVar3 = iVar3 + 1;
    vu0CopyVector(auStack_60,puVar5);
    vu0MulMatrix(auStack_d0,param_1,auStack_90);
    vifObjectMatrix(auStack_d0);
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)piVar4));
    piVar4 = piVar4 + 0x4;  // T* element step
    vifCallModel(iVar1 + 0x10);
    puVar5 = puVar5 + 0x10;
  } while (iVar3 < 3);
  return 0;
}


RT_GUEST_FN(0x002213e8, guest::drawCarWheelsMid)

}  // namespace guest
