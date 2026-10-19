// guest:0x00241ba8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateMapCoinCollection(uint32_t param_1, uint32_t param_2, ...) {
  long lVar1;
  ulong uVar2;
  int iVar3;
  ulong uVar4;
  int iVar5;
  float fVar7;
uint32_t auStack_50 = rtguest::allocFrame(16);
  ulong uVar6;
  
  if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) < 0x40) {
    uiFade((0x40 - (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2)))) * 0x2000000);
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) + 1;
  }
  iVar5 = 1;
  uVar4 = 0;
  do {
    uVar6 = (uint32_t)iVar5;
    lVar1 = TestFlagFca8Clear(uVar6);
    iVar3 = (int)uVar4 * 0x10;
    if ((lVar1 == 0) && ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002a397c + iVar3))) == (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x54))))) {
      vu0SubVector(auStack_50,0x177ace0,0x2a3970 + iVar3);
      fVar7 = (float)vecDotProductVu0(auStack_50,auStack_50);
      if (SQRT(fVar7) < (*(float*)rtguest::G(0x332eec))) {
        PlayUiSound(0x29);
        uVar2 = uVar4;
        if ((long)uVar4 < 0) {
          uVar2 = (long)((int)uVar4 + 0x3f);
        }
        (*(uint64_t*)rtguest::G((uint32_t)0x177fca8 + (uint32_t)((int)uVar2 >> 6) * 8)) =
             (*(uint64_t*)rtguest::G((uint32_t)0x177fca8 + (uint32_t)((int)uVar2 >> 6) * 8)) & ~(1L << (uVar4 & 0x3f));
        lVar1 = CountBitsFca8Fcb0(0);
        if (lVar1 == 0x46) {
          AppendQueuedByte(0x7e);
        }
      }
    }
    iVar5 = iVar5 + 1;
    uVar4 = uVar6;
  } while ((long)uVar6 < 100);
  return 0;
}


RT_GUEST_FN(0x00241ba8, guest::UpdateMapCoinCollection)

}  // namespace guest
