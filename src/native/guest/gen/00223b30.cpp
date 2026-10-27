// guest:0x00223b30 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawCarEffectB(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
  rtguest::FrameGuard rt_fg(224);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x90, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0xa0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x98, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0xa8, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0xb0, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0xb8, 8);
    rtguest::rtSpillF(rt_fg, 5, rt_sv, 0xd0);
    rtguest::rtSpillF(rt_fg, 4, rt_sv, 0xc8);
    *(uint32_t*)rtguest::G(rt_sv+0xc0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc4) = 0x00000000u;
    rtguest::gS[0] = rt_sv + 0x80ull;  // walk: sp+0x80
    rtguest::setS32(1, (uint32_t)param_4);  // walk: = param_4
    rtguest::setS32(2, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(4, (uint32_t)param_3);  // walk: = param_3
  }
  ulong uVar1;
  long lVar2;
uint32_t auStack_e0 = rtguest::allocFrame(16);
uint32_t auStack_d0 = rtguest::allocFrame(16);
uint32_t auStack_c0 = rtguest::allocFrame(32);
uint32_t auStack_a0 = rtguest::allocFrame(16);
uint32_t auStack_90 = rtguest::allocFrame(16);
uint32_t auStack_80 = rtguest::allocFrame(16);
uint32_t auStack_70 = rtguest::allocFrame(16);
uint32_t auStack_60 = rtguest::allocFrame(16);
  
  RT_CALLRA(0x223b70, guest::vu0SubVector(auStack_60, param_2, param_3));  // W13c19: fn/00223b30.c:21 drops a1/a2; MIPS 0x223b44 move $4,$16 only; $5,$6 (move $20,$6 @0x223b58) pass through to jal 0x272958 @0x223b70
  RT_CALLRA(0x223b7c, guest::vecNormalizeVu0(auStack_60,auStack_60));
  RT_CALLRA(0x223b90, guest::BuildTransformMatrix(auStack_e0,0x29d3a0,auStack_60));
  (*(uint32_t*)rtguest::G(0x29d37c)) = param_2;
  RT_CALLRA(0x223ba8, guest::vifUploadColours(0x29d370,0x29d360));
  RT_CALLRA(0x223bb4, guest::vu0UnitMatrix(auStack_a0));
  RT_CALLRA(0x223bc4, guest::sceVu0ScaleVectorXYZ(rtguest::fbits(param_1),auStack_a0,auStack_e0));
  RT_CALLRA(0x223bd4, guest::sceVu0ScaleVectorXYZ(rtguest::fbits(param_1),auStack_90,auStack_d0));
  RT_CALLRA(0x223be4, guest::sceVu0ScaleVectorXYZ(rtguest::fbits(param_1),auStack_80,auStack_c0));
  RT_CALLRA(0x223bf4, guest::vu0CopyVectorXYZ(auStack_70,param_5));
  RT_CALLRA(0x223bfc, guest::vifObjectMatrix(auStack_a0));
  if (param_6 == 2) {
    lVar2 = (long)(param_3 * 3 + 0x3f7c);
    uVar1 = (uint32_t)(param_3 * 3 + 0x3f7a);
  }
  else if (param_6 < 3) {
    if (param_6 != 1) goto LAB_00223d08;
    lVar2 = (long)(param_3 * 3 + 0x3f4c);
    uVar1 = (uint32_t)(param_3 * 3 + 0x3f4a);
  }
  else {
    if (param_6 != 3) {
      if (param_6 == 7) {
        RT_CALLRA(0x223cc4, guest::vifBatchRegs(0x2007e5655540bf29,7,5,9));
      }
      goto LAB_00223d08;
    }
    lVar2 = (long)(param_3 * 3 + 0x3f6a);
    uVar1 = (uint32_t)(param_3 * 3 + 0x3f68);
  }
  RT_CALLRA(0x223cf8, guest::vifBatchRegs(uVar1 | 0x15408000 | lVar2 << 0x25 | 0x2000000540000000,7,5,9));
LAB_00223d08:
  RT_CALLRA(0x223d08, guest::vifCallModel(0x290770));
  RT_CALLRA(0x223d14, guest::vu0CopyVectorXYZ(auStack_70,(int)param_5 + 0x10));
  RT_CALLRA(0x223d1c, guest::vifObjectMatrix(auStack_a0));
  RT_CALLRA(0x223d24, guest::vifCallModel(0x290770));
  return 0;
}


RT_GUEST_FN(0x00223b30, guest::drawCarEffectB)

}  // namespace guest
