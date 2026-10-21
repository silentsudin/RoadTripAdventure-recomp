// guest:0x002249b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SpawnCarDebrisCycle(uint32_t param_1, uint32_t param_2, uint64_t param_3, ...) {  // param_3 = $a2 (move $18,$6 at 0x2249d4; caller 0x225120 passes it, Ghidra dropped it)
  rtguest::FrameGuard rt_fg(176);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x98) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x9c) = 0x00000000u;
  }
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
  if (0.0f < (*(float*)rtguest::G(0x172ff7c))) {
    iVar3 = 2;
    while ((pfVar1 = pfVar1 + 0x20, iVar3 < 0x80 && (0.0 < (*(float*)rtguest::G((uint32_t)pfVar1))))) {
      iVar3 = iVar3 + 2;
    }
  }
  if (iVar3 != 0x80) {
    fVar5 = rtguest::fbits(RT_CALLRA(0x224a48, guest::GetSpeedFromF15Vector((int)param_1 + 0xf0)));
    fVar4 = rtguest::fdiv(fVar5, 300.0);
    if (1.0 < rtguest::fdiv(fVar5, 300.0)) {
      fVar4 = 1.0;
    }
    fVar5 = rtguest::fbits(RT_CALLRA(0x224a94, guest::__floatdisf(((param_3 + 4) & 0xf))));  // MIPS daddiu $16,$18,4 @0x224a84; andi $4,$16,0xf @0x224a88
    (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(0) * 4)) = (*(float*)rtguest::G(0x332d80)) - fVar5 * 0.0625;
    fVar5 = rtguest::fbits(RT_CALLRA(0x224adc, guest::__floatdisf((param_3 & 0xf))));  // MIPS andi $4,$18,0xf @0x224ac4
    (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(1) * 4)) = 0.5;
    (*(float*)rtguest::G((uint32_t)afStack_60 + (uint32_t)(1) * 4)) = 0.5;
    (*(float*)rtguest::G((uint32_t)afStack_60 + (uint32_t)(0) * 4)) = fVar5 * 0.0625 + (*(float*)rtguest::G(0x332d84));
    fVar5 = rtguest::fbits(RT_CALLRA(0x224b38, guest::__floatdisf(((param_3 + 4) & 7))));  // MIPS andi $4,$16,7 @0x224b0c
    (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(2) * 4)) = 1.5 - fVar5 * 0.375;
    fVar5 = rtguest::fbits(RT_CALLRA(0x224b84, guest::__floatdisf((param_3 & 7))));  // MIPS andi $4,$18,7 @0x224b64
    (*(uint32_t*)rtguest::G((uint32_t)uStack_64)) = 0x3f800000;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_54)) = 0x3f800000;
    (*(float*)rtguest::G((uint32_t)afStack_60 + (uint32_t)(2) * 4)) = 1.5 - fVar5 * 0.375;
    RT_CALLRA(0x224be4, guest::vu0ApplyMatrix(afStack_70,param_1,afStack_70));
    RT_CALLRA(0x224bf8, guest::vu0ApplyMatrix(afStack_60,param_1,afStack_60));
    RT_CALLRA(0x224c04, guest::vu0CopyVector(auStack_80,afStack_70));
    RT_CALLRA(0x224c14, guest::vu0MulMatrix(auStack_b0,param_2,auStack_b0));
    if (0.0 < (*(float*)rtguest::G((uint32_t)fStack_74))) {
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x20c))) = (short)iVar3;
      iVar2 = iVar3 * 0x10;
      (*(uint8_t*)rtguest::G((uint32_t)0x1730770 + (uint32_t)(iVar3))) = 8;
      (*(uint8_t*)rtguest::G((uint32_t)0x17307f4 + (uint32_t)(iVar3 * 8))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017307f0 + iVar3 * 8))) = 0;
      RT_CALLRA(0x224c70, guest::vu0CopyVector(0x0172ff70 + iVar2,afStack_70));
      RT_CALLRA(0x224c80, guest::vu0CopyVector(iVar2 + 0x172ff80,afStack_60));
      (*(uint32_t*)rtguest::G((uint32_t)0x172ff7c + (uint32_t)(iVar3 * 4) * 4)) = rtguest::f2bits(fVar4 + 0.25f);
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0172ff8c + iVar2))) = 0x3f000000;
    }
  }
  return 0;
}


RT_GUEST_FN(0x002249b8, guest::SpawnCarDebrisCycle)

}  // namespace guest
