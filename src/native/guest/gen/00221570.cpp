// guest:0x00221570 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawCarWheelsNear(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
uint32_t auStack_120 = rtguest::allocFrame(48);
uint32_t auStack_f0 = rtguest::allocFrame(16);
uint32_t auStack_e0 = rtguest::allocFrame(64);
uint32_t auStack_a0 = rtguest::allocFrame(64);
    uint32_t uStack_60 = rtguest::allocFrame(4);
    uint32_t uStack_5c = rtguest::allocFrame(4);
    uint32_t uStack_58 = rtguest::allocFrame(4);
    uint32_t uStack_54 = rtguest::allocFrame(4);
uint32_t puVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  
  iVar2 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 5))) * 3;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_60)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x29d150 + (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x29d1a0))) * 4)));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_58)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x29d150 + (uint32_t)(uint32_t)(*(uint8_t*)rtguest::G((uint32_t)0x29d1a2 + (uint32_t)(iVar2) * 1)) * 4)));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x29d150 + (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x29d1a1))) * 4)));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_54)) = 0;
  vifUploadColours(uStack_60,uStack_60);
  vu0UnitMatrix(auStack_a0);
  vu0RotMatrixZ((*(uint32_t*)rtguest::G(0x332cbc)),auStack_e0,auStack_a0);
  iVar2 = 0;
  do {
    if (iVar2 == 1) {
      uVar4 = (*(uint32_t*)rtguest::G((uint32_t)param_2));
      puVar1 = auStack_e0;
LAB_002216ac:
      vu0RotMatrixX(uVar4,auStack_120,puVar1);
      vu0RotMatrixY((*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(uint32_t))),auStack_120,auStack_120);
    }
    else if (iVar2 < 2) {
      if (iVar2 == 0) {
        uVar4 = (*(uint32_t*)rtguest::G((uint32_t)param_2));
        puVar1 = auStack_a0;
        goto LAB_002216ac;
      }
    }
    else if (iVar2 == 2) {
      vu0RotMatrixX((*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(uint32_t))),auStack_120,auStack_a0);
    }
    else if (iVar2 == 3) {
      vu0RotMatrixX((*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(uint32_t))),auStack_120,auStack_e0);
    }
    iVar3 = iVar2 + 1;
    vu0CopyVector(auStack_f0,iVar2 * 0x10 + 0x29cb60);
    vu0MulMatrix(auStack_120,param_1,auStack_120);
    vifObjectMatrix(auStack_120);
    vifCallModel((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ab34 + (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 6))) * 4))) + 0x10);
    vifCallModel((*(uint32_t*)rtguest::G(0x177ab30)) + 0x10);
    iVar2 = iVar3;
    if (3 < iVar3) {
      return 0;
    }
  } while( true );
}


RT_GUEST_FN(0x00221570, guest::drawCarWheelsNear)

}  // namespace guest
