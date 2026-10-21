// guest:0x00224ec0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t AdvanceCarEmitterStep(uint32_t rt_bits_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, uint32_t param_7, ...) {
  float param_1 = rtguest::fbits(rt_bits_1);  // f12.. float arg carried as bits (W13 float-param fix)
  rtguest::FrameGuard rt_fg(192);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x68, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x78, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x80, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x88, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x90, 8);
    rtguest::rtSpillF(rt_fg, 6, rt_sv, 0xb0);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x70, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x98) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x9c) = 0x00000000u;
    rtguest::rtSpillF(rt_fg, 5, rt_sv, 0xa8);
    rtguest::rtSpillF(rt_fg, 4, rt_sv, 0xa0);
    rtguest::setS32(0, (uint32_t)param_4);  // walk: = param_4
    rtguest::setS32(3, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(4, (uint32_t)param_3);  // walk: = param_3
    rtguest::setS32(5, (uint32_t)param_6);  // walk: = param_6
    rtguest::setS32(6, (uint32_t)param_5);  // walk: = param_5
  }
    uint32_t uStack_c0 = rtguest::allocFrame(4);
    uint32_t uStack_bc = rtguest::allocFrame(4);
    uint32_t fStack_b8 = rtguest::allocFrame(4);
    uint32_t uStack_b4 = rtguest::allocFrame(4);
    uint32_t uStack_b0 = rtguest::allocFrame(4);
    uint32_t uStack_ac = rtguest::allocFrame(4);
    uint32_t fStack_a8 = rtguest::allocFrame(4);
    uint32_t uStack_a4 = rtguest::allocFrame(4);
uint32_t auStack_a0 = rtguest::allocFrame(48);
uint32_t auStack_70 = rtguest::allocFrame(12);
    uint32_t fStack_64 = rtguest::allocFrame(4);
uint32_t pfVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  float fVar5;
  
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_5));
  if (0x10 < iVar2) {
    (*(int32_t*)rtguest::G((uint32_t)param_5)) = 0;
    iVar2 = 0;
  }
  if (iVar2 < 1) {
    (*(int32_t*)rtguest::G((uint32_t)param_5)) = 0;
    fVar5 = 0.0;
    uVar4 = 0;
  }
  else {
    (*(float*)rtguest::G((uint32_t)fStack_b8)) = (*(float*)rtguest::G(0x332d90)) - (float)iVar2 * (*(float*)rtguest::G(0x332d8c));
    fVar5 = (float)iVar2 * 0.0078125 + 0.0078125;
    uVar4 = 0x3f000000;
    (*(float*)rtguest::G((uint32_t)fStack_a8)) = (*(float*)rtguest::G((uint32_t)fStack_b8));
    if (iVar2 == 0x10) {
      (*(int32_t*)rtguest::G((uint32_t)param_5)) = 0;
      uVar4 = 0x3f000000;
    }
  }
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c0)) = (*(uint32_t*)rtguest::G(0x332d94));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b0)) = (*(uint32_t*)rtguest::G(0x332d98));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_a4)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_bc)) = 0;
  RT_CALLRA(0x224fb4, guest::vu0ApplyMatrix(uStack_c0,param_2,uStack_c0));
  RT_CALLRA(0x224fc8, guest::vu0ApplyMatrix(uStack_b0,param_2,uStack_b0));
  if (((*(int32_t*)rtguest::G((uint32_t)param_5)) == 0) ||
     (drawCarEffectB(fVar5,uVar4,param_3 >> 2 & 3,param_4,uStack_c0,7), (*(int32_t*)rtguest::G((uint32_t)param_5)) == 0)) {
    RT_CALLRA(0x22500c, guest::vu0CopyVector(auStack_70,uStack_c0));
    RT_CALLRA(0x22501c, guest::vu0MulMatrix(auStack_a0,param_7,auStack_a0));
    if (0.0 < (*(float*)rtguest::G((uint32_t)fStack_64))) {
      pfVar1 = 0x0172ff7c;
      iVar2 = 0;
      if (0.0 < (*(float*)rtguest::G(0x172ff7c))) {
        iVar2 = 2;
        while ((pfVar1 = pfVar1 + 0x20, iVar2 < 0x80 && (0.0 < (*(float*)rtguest::G((uint32_t)pfVar1))))) {
          iVar2 = iVar2 + 2;
        }
      }
      iVar3 = iVar2 * 0x10;
      if (iVar2 != 0x80) {
        (*(uint16_t*)rtguest::G((uint32_t)param_6)) = (short)iVar2;
        (*(uint8_t*)rtguest::G((uint32_t)0x1730770 + (uint32_t)(iVar2))) = 7;
        RT_CALLRA(0x2250b4, guest::vu0CopyVector(0x0172ff70 + iVar3,uStack_c0));
        RT_CALLRA(0x2250c4, guest::vu0CopyVector(iVar3 + 0x172ff80,uStack_b0));
        (*(float*)rtguest::G((uint32_t)0x172ff7c + (uint32_t)(iVar2 * 4) * 4)) = fVar5;
        (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff8c + iVar3))) = param_1 - 0.5;
      }
    }
  }
  return 0;
}


RT_GUEST_FN(0x00224ec0, guest::AdvanceCarEmitterStep)

}  // namespace guest
