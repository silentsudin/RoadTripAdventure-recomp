// guest:0x00241d38 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawCoins(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(144);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x80) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x84) = 0x00000000u;
  }
  uint uVar1;
  int iVar2;
  long lVar3;
  int iVar4;
  int iVar5;
uint32_t auStack_90 = rtguest::allocFrame(48);
uint32_t auStack_60 = rtguest::allocFrame(16);
  
  RT_CALLRA(0x241d6c, guest::vu0UnitMatrix(auStack_90));
  RT_CALLRA(0x241da4, guest::vu0RotMatrixY((float)(int)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) - 0x80) * (*(float*)rtguest::G(0x332ef0)) * 0.0078125,auStack_90,
                auStack_90));
  RT_CALLRA(0x241dcc, guest::vifFixedRegs(0x2003aa0599405d48,7,0,0xf));
  iVar2 = 1;
  iVar4 = 0;
  do {
    iVar5 = iVar2;
    lVar3 = RT_CALLRA(0x241de8, guest::TestFlagFca8Clear(iVar5));
    if (lVar3 == 0) {
      uVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002a397c + iVar4 * 0x10)));
      lVar3 = RT_CALLRA(0x241e08, guest::tileSlotLoaded(param_2,uVar1));
      if (lVar3 != 0) {
        RT_CALLRA(0x241e1c, guest::vu0CopyVectorXYZ(auStack_60,0x2a3970 + iVar4 * 0x10));
        lVar3 = RT_CALLRA(0x241e2c, guest::tileSlotOrigin(auStack_60,auStack_60,uVar1 & 3));
        if (lVar3 != 0) {
          RT_CALLRA(0x241e3c, guest::vifObjectMatrix(auStack_90));
          RT_CALLRA(0x241e48, guest::vifCallModel((*(uint32_t*)rtguest::G(0x335718)) + 0x10));
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
