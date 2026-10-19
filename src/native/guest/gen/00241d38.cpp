// guest:0x00241d38 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawCoins(uint32_t param_1, uint32_t param_2, ...) {
  uint uVar1;
  int iVar2;
  long lVar3;
  int iVar4;
  int iVar5;
uint32_t auStack_90 = rtguest::allocFrame(48);
uint32_t auStack_60 = rtguest::allocFrame(16);
  
  vu0UnitMatrix(auStack_90);
  vu0RotMatrixY((float)(int)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) - 0x80) * (*(float*)rtguest::G(0x332ef0)) * 0.0078125,auStack_90,
                auStack_90);
  vifFixedRegs(0x2003aa0599405d48,7,0,0xf);
  iVar2 = 1;
  iVar4 = 0;
  do {
    iVar5 = iVar2;
    lVar3 = TestFlagFca8Clear(iVar5);
    if (lVar3 == 0) {
      uVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002a397c + iVar4 * 0x10)));
      lVar3 = tileSlotLoaded(param_2,uVar1);
      if (lVar3 != 0) {
        vu0CopyVectorXYZ(auStack_60,0x2a3970 + iVar4 * 0x10);
        lVar3 = tileSlotOrigin(auStack_60,auStack_60,uVar1 & 3);
        if (lVar3 != 0) {
          vifObjectMatrix(auStack_90);
          vifCallModel((*(uint32_t*)rtguest::G(0x335718)) + 0x10);
        }
      }
    }
    iVar2 = iVar5 + 1;
    iVar4 = iVar5;
  } while (iVar5 < 100);
  return 0;
}


RT_GUEST_FN(0x00241d38, guest::drawCoins)

}  // namespace guest
