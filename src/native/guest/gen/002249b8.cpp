// guest:0x002249b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SpawnCarDebrisCycle(uint32_t param_1, uint32_t param_2, ...) {
uint32_t auStack_b0 = rtguest::allocFrame(48);
uint32_t auStack_80 = rtguest::allocFrame(12);
    uint32_t fStack_74 = rtguest::allocFrame(4);
uint32_t afStack_70 = rtguest::allocFrame(12);
    uint32_t uStack_64 = rtguest::allocFrame(4);
uint32_t afStack_60 = rtguest::allocFrame(12);
    uint32_t uStack_54 = rtguest::allocFrame(4);
uint32_t pfVar1;
  int iVar2;
  int iVar3;
  float fVar4;
  float fVar5;
  
  iVar3 = 0;
  pfVar1 = 0x0172ff7c;
  if (0.0 < (*(uint32_t*)rtguest::G(0x172ff7c))) {
    iVar3 = 2;
    while ((pfVar1 = pfVar1 + 0x20, iVar3 < 0x80 && (0.0 < (*(float*)rtguest::G((uint32_t)pfVar1))))) {
      iVar3 = iVar3 + 2;
    }
  }
  if (iVar3 != 0x80) {
    fVar5 = rtguest::fbits(GetSpeedFromF15Vector((int)param_1 + 0xf0));
    fVar4 = fVar5 / 300.0;
    if (1.0 < fVar5 / 300.0) {
      fVar4 = 1.0;
    }
    fVar5 = (float)__floatdisf();
    (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(0) * 4)) = (*(uint32_t*)rtguest::G(0x332d80)) - fVar5 * 0.0625;
    fVar5 = (float)__floatdisf();
    (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(1) * 4)) = 0.5;
    (*(float*)rtguest::G((uint32_t)afStack_60 + (uint32_t)(1) * 4)) = 0.5;
    (*(float*)rtguest::G((uint32_t)afStack_60 + (uint32_t)(0) * 4)) = fVar5 * 0.0625 + (*(uint32_t*)rtguest::G(0x332d84));
    fVar5 = (float)__floatdisf();
    (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(2) * 4)) = 1.5 - fVar5 * 0.375;
    fVar5 = (float)__floatdisf();
    (*(uint32_t*)rtguest::G((uint32_t)uStack_64)) = 0x3f800000;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_54)) = 0x3f800000;
    (*(float*)rtguest::G((uint32_t)afStack_60 + (uint32_t)(2) * 4)) = 1.5 - fVar5 * 0.375;
    vu0ApplyMatrix(afStack_70,param_1,afStack_70);
    vu0ApplyMatrix(afStack_60,param_1,afStack_60);
    vu0CopyVector(auStack_80,afStack_70);
    vu0MulMatrix(auStack_b0,param_2,auStack_b0);
    if (0.0 < (*(float*)rtguest::G((uint32_t)fStack_74))) {
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x20c))) = (short)iVar3;
      iVar2 = iVar3 * 0x10;
      (*(uint32_t*)rtguest::G((uint32_t)0x1730770 + (uint32_t)(iVar3) * 4)) = 8;
      (*(uint32_t*)rtguest::G((uint32_t)0x17307f4 + (uint32_t)(iVar3 * 8) * 4)) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017307f0 + iVar3 * 8))) = 0;
      vu0CopyVector(0x0172ff70 + iVar2,afStack_70);
      vu0CopyVector(iVar2 + 0x172ff80,afStack_60);
      (*(uint32_t*)rtguest::G((uint32_t)0x172ff7c + (uint32_t)(iVar3 * 4) * 4)) = fVar4 + 0.25;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0172ff8c + iVar2))) = 0x3f000000;
    }
  }
  return 0;
}


RT_GUEST_FN(0x002249b8, guest::SpawnCarDebrisCycle)

}  // namespace guest
