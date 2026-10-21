// guest:0x00224cd0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SpawnCarEmitterEntry(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(176);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x98) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x9c) = 0x00000000u;
  }
uint32_t auStack_b0 = rtguest::allocFrame(48);
uint32_t auStack_80 = rtguest::allocFrame(12);
    uint32_t fStack_74 = rtguest::allocFrame(4);
    uint32_t uStack_70 = rtguest::allocFrame(4);
    uint32_t uStack_6c = rtguest::allocFrame(4);
    uint32_t uStack_68 = rtguest::allocFrame(4);
    uint32_t uStack_64 = rtguest::allocFrame(4);
uint32_t auStack_60 = rtguest::allocFrame(16);
uint32_t pfVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  float fVar5;
  float fVar6;
  
  iVar4 = 0;
  pfVar1 = 0x0172ff7c;
  if (0.0 < (*(float*)rtguest::G(0x172ff7c))) {
    iVar4 = 2;
    while ((pfVar1 = pfVar1 + 0x20, iVar4 < 0x80 && (0.0 < (*(float*)rtguest::G((uint32_t)pfVar1))))) {
      iVar4 = iVar4 + 2;
    }
  }
  if (iVar4 != 0x80) {
    fVar5 = rtguest::fbits(RT_CALLRA(0x224d60, guest::GetSpeedFromF15Vector((int)param_1 + 0xf0)));
    fVar6 = rtguest::fdiv(fVar5, 300.0);
    if (1.0 < rtguest::fdiv(fVar5, 300.0)) {
      fVar6 = 1.0;
    }
    (*(uint32_t*)rtguest::G((uint32_t)uStack_6c)) = (*(uint32_t*)rtguest::G(0x332d88));
    (*(uint32_t*)rtguest::G((uint32_t)uStack_68)) = 0x3fe00000;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_64)) = 0x3f800000;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_70)) = 0;
    RT_CALLRA(0x224dc8, guest::vu0CopyVector(auStack_60,uStack_70));
    RT_CALLRA(0x224dd8, guest::vu0ApplyMatrix(uStack_70,param_1,uStack_70));
    RT_CALLRA(0x224de8, guest::vu0ApplyMatrix(auStack_60,param_1,auStack_60));
    RT_CALLRA(0x224df4, guest::vu0CopyVector(auStack_80,uStack_70));
    RT_CALLRA(0x224e04, guest::vu0MulMatrix(auStack_b0,param_2,auStack_b0));
    if (0.0 < (*(float*)rtguest::G((uint32_t)fStack_74))) {
      iVar2 = iVar4 * 8;
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x20c))) = (short)iVar4;
      iVar3 = iVar4 * 0x10;
      (*(uint32_t*)rtguest::G((uint32_t)0x1730770 + (uint32_t)(iVar4) * 4)) = 8;
      (*(uint32_t*)rtguest::G((uint32_t)0x17307f4 + (uint32_t)(iVar2) * 4)) = 2;
      (*(uint32_t*)rtguest::G((uint32_t)0x17307f5 + (uint32_t)(iVar2) * 4)) = param_4;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017307f0 + iVar2))) = 0;
      RT_CALLRA(0x224e64, guest::vu0CopyVectorXYZ(0x0172ff70 + iVar3,uStack_70));
      RT_CALLRA(0x224e74, guest::vu0CopyVectorXYZ(iVar3 + 0x172ff80,auStack_60));
      (*(float*)rtguest::G((uint32_t)0x172ff7c + (uint32_t)(iVar4 * 4) * 4)) = fVar6 + 0.5;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0172ff8c + iVar3))) = 0x3e800000;
    }
  }
  return 0;
}


RT_GUEST_FN(0x00224cd0, guest::SpawnCarEmitterEntry)

}  // namespace guest
