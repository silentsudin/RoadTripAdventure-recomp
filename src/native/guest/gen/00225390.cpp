// guest:0x00225390 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t AdvanceCarEffectSlot(uint32_t param_1, ...) {
    uint32_t fStack_50 = rtguest::allocFrame(4);
    uint32_t fStack_4c = rtguest::allocFrame(4);
    uint32_t fStack_48 = rtguest::allocFrame(4);
    uint32_t uStack_44 = rtguest::allocFrame(4);
    uint32_t fStack_40 = rtguest::allocFrame(4);
    uint32_t fStack_3c = rtguest::allocFrame(4);
    uint32_t fStack_38 = rtguest::allocFrame(4);
    uint32_t uStack_34 = rtguest::allocFrame(4);
  char cVar1;
  int iVar2;
  int iVar3;
uint32_t puVar4;
  float fVar5;
  
  iVar2 = param_1 * 8;
  puVar4 = (uint32_t)(0x017307f0 + iVar2);
  iVar3 = param_1 * 0x10;
  cVar1 = (*(int8_t*)rtguest::G((uint32_t)0x17307f5 + iVar2));
  if ((*(int8_t*)rtguest::G((uint32_t)0x17307f4 + iVar2)) == '\x01') {
    fVar5 = (*(float*)rtguest::G((uint32_t)0x172ff7c + param_1 * 4));
    (*(float*)rtguest::G((uint32_t)0x172ff7c + param_1 * 4)) = fVar5 - 0.03125;
    if (fVar5 - 0.03125 < 0.0) {
      (*(uint8_t*)rtguest::G((uint32_t)0x1730770 + param_1)) = 0;
      (*(float*)rtguest::G((uint32_t)0x172ff7c + param_1 * 4)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)puVar4)) = 0;
      (*(uint8_t*)rtguest::G((uint32_t)0x17307f4 + iVar2)) = 0;
    }
    fVar5 = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff8c + iVar3))) + 0.25;
  }
  else {
    if ((*(int8_t*)rtguest::G((uint32_t)0x17307f4 + iVar2)) != '\x02') {
      return 0;
    }
    fVar5 = (*(float*)rtguest::G((uint32_t)0x172ff7c + param_1 * 4));
    (*(float*)rtguest::G((uint32_t)0x172ff7c + param_1 * 4)) = fVar5 - 0.0625;
    if (fVar5 - 0.0625 < 0.0) {
      (*(uint8_t*)rtguest::G((uint32_t)0x1730770 + param_1)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0172ff8c + iVar3))) = 0;
      (*(float*)rtguest::G((uint32_t)0x172ff7c + param_1 * 4)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)puVar4)) = 0;
      (*(uint8_t*)rtguest::G((uint32_t)0x17307f4 + iVar2)) = 0;
      return 0;
    }
    fVar5 = (float)((*(uint32_t*)rtguest::G((uint32_t)puVar4)) & 0xf);
    (*(uint32_t*)rtguest::G((uint32_t)uStack_44)) = 0x3f800000;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_34)) = 0x3f800000;
    (*(float*)rtguest::G((uint32_t)fStack_48)) = 1.75 - fVar5 * (*(float*)rtguest::G(0x332d9c));
    (*(float*)rtguest::G((uint32_t)fStack_40)) = 0.0 - fVar5 * 0.09375;
    (*(float*)rtguest::G((uint32_t)fStack_4c)) = fVar5 * 0.015625 + (*(float*)rtguest::G(0x332da0));
    (*(float*)rtguest::G((uint32_t)fStack_50)) = fVar5 * 0.09375 + 0.0;
    (*(float*)rtguest::G((uint32_t)fStack_3c)) = (*(float*)rtguest::G((uint32_t)fStack_4c));
    (*(float*)rtguest::G((uint32_t)fStack_38)) = (*(float*)rtguest::G((uint32_t)fStack_48));
    vu0ApplyMatrix(fStack_50,0x177ac50 + (cVar1 * 0x28 - (int)cVar1) * 2,fStack_50);
    vu0ApplyMatrix(fStack_40,0x177ac50 + (cVar1 * 0x28 - (int)cVar1) * 2,fStack_40);
    vu0CopyVectorXYZ(0x0172ff70 + iVar3,fStack_50);
    vu0CopyVectorXYZ(iVar3 + 0x172ff80,fStack_40);
    fVar5 = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff8c + iVar3))) + 0.0625;
  }
  (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff8c + iVar3))) = fVar5;
  return 0;
}


RT_GUEST_FN(0x00225390, guest::AdvanceCarEffectSlot)

}  // namespace guest
