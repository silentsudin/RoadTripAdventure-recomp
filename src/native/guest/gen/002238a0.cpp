// guest:0x002238a0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t drawCarEffectA(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(352);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x148) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14c) = 0x00000000u;
  }
uint32_t auStack_160 = rtguest::allocFrame(48);
    uint32_t fStack_130 = rtguest::allocFrame(4);
    uint32_t uStack_12c = rtguest::allocFrame(4);
    uint32_t uStack_128 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
uint32_t afStack_120 = rtguest::allocFrame(20);
    uint32_t fStack_10c = rtguest::allocFrame(4);
    rtguest::allocFrame(16);  // guest frame gap
    uint32_t fStack_f8 = rtguest::allocFrame(4);
    rtguest::allocFrame(20);  // guest frame gap
uint32_t auStack_e0 = rtguest::allocFrame(64);
uint32_t auStack_a0 = rtguest::allocFrame(64);
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  float fVar5;
  float fVar6;
  
  iVar2 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G((uint32_t)0x177ea50 + (uint32_t)(param_3) * 4)) + 4))) * 0x44;
  if (param_2 < 0) {
    fVar5 = (float)param_2;
    param_2 = 0x14 - param_2;
    (*(float*)rtguest::G(0x29d37c)) = fVar5 * (*(float*)rtguest::G(0x332d00));
  }
  else {
    (*(uint32_t*)rtguest::G(0x29d37c)) = rtguest::f2bits(0.5f);
  }
  RT_CALLRA(0x223950, guest::vifUploadColours(0x29d370,0x29d360));
  RT_CALLRA(0x22397c, guest::vu0MulMatrix(auStack_e0,param_1,(int)param_1 + 0x40));
  RT_CALLRA(0x223988, guest::vu0UnitMatrix(afStack_120));
  if (param_2 < 8) {
    (*(float*)rtguest::G((uint32_t)fStack_f8)) = (float)param_2 * 0.03125 + 0.75;
    (*(float*)rtguest::G((uint32_t)afStack_120 + (uint32_t)(0) * 4)) = (float)param_2 * 0.0625 + 0.5;
    (*(float*)rtguest::G((uint32_t)fStack_10c)) = (*(float*)rtguest::G((uint32_t)afStack_120 + (uint32_t)(0) * 4));
  }
  iVar1 = (param_2 % 6) * 5;
  iVar4 = -4;
  iVar3 = (param_2 % 6 & 1U) - 8;
  fVar5 = (*(float*)rtguest::G(0x332d04));
  do {
    fVar6 = (float)iVar3;
    iVar3 = iVar3 + 2;
    RT_CALLRA(0x223a4c, guest::vu0RotMatrixZ(fVar6 * fVar5 * 0.125,auStack_160,afStack_120));
    (*(float*)rtguest::G((uint32_t)fStack_130)) = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002ed758 + iVar2)));
    (*(uint32_t*)rtguest::G((uint32_t)uStack_12c)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed75c + iVar2)));
    (*(uint32_t*)rtguest::G((uint32_t)uStack_128)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed760 + iVar2)));
    RT_CALLRA(0x223a74, guest::vu0MulMatrix(auStack_a0,auStack_e0,auStack_160));
    RT_CALLRA(0x223a7c, guest::vifObjectMatrix(auStack_a0));
    if (iVar4 == -4) {
      RT_CALLRA(0x223ab0, guest::vifBatchRegs((long)(iVar1 + 0x3f2c) | 0x19408000U | (long)(iVar1 + 0x3f30) << 0x25 |
                   0x2000000540000000,7,5,9));
    }
    iVar4 = iVar4 + 1;
    RT_CALLRA(0x223abc, guest::vifCallModel(0x290670));
    (*(float*)rtguest::G((uint32_t)fStack_130)) = -(*(float*)rtguest::G((uint32_t)fStack_130));
    RT_CALLRA(0x223ad8, guest::vu0MulMatrix(auStack_a0,auStack_e0,auStack_160));
    RT_CALLRA(0x223ae0, guest::vifObjectMatrix(auStack_a0));
    RT_CALLRA(0x223ae8, guest::vifCallModel(0x290670));
  } while (iVar4 < 4);
  return 0;
}


RT_GUEST_FN(0x002238a0, guest::drawCarEffectA)

}  // namespace guest
