// guest:0x00242d20 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t raceMapMarkers(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(368);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x100, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x108, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x110, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x118, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x120, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x128, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x130, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x138, 8);
    rtguest::rtSpillF(rt_fg, 7, rt_sv, 0x160);
    rtguest::rtSpillF(rt_fg, 6, rt_sv, 0x158);
    rtguest::rtSpillF(rt_fg, 5, rt_sv, 0x150);
    rtguest::rtSpillF(rt_fg, 4, rt_sv, 0x148);
    *(uint32_t*)rtguest::G(rt_sv+0x140) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x144) = 0x00000000u;
    rtguest::gS[0] = 0x000000000177ace0ull;  // walk const
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
    rtguest::gS[2] = 0x000000000177ac50ull;  // walk const
    rtguest::gS[3] = 0x0000000000000001ull;  // walk const
    rtguest::gS[4] = 0x00000000002a4360ull;  // walk const
    rtguest::gS[6] = 0x00000000002a0000ull;  // walk const
    rtguest::gS[7] = 0x00000000002a0000ull;  // walk const
  }
uint32_t afStack_170 = rtguest::allocFrame(40);
    uint32_t fStack_148 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_140 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_138 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
uint32_t afStack_130 = rtguest::allocFrame(40);
    uint32_t fStack_108 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_100 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_f8 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
uint32_t afStack_f0 = rtguest::allocFrame(40);
    uint32_t fStack_c8 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_c0 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_b8 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
uint32_t afStack_b0 = rtguest::allocFrame(40);
    uint32_t fStack_88 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_80 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_78 = rtguest::allocFrame(4);
uint32_t puVar1;
  uint uVar2;
  int iVar3;
  float fVar4;
  float fVar5;
  
  uVar2 = 1;
  fVar4 = rtguest::fdiv(50.0, (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002a4058 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x22))) * 0x10))));
  fVar5 = (*(float*)rtguest::G(0x332f34));
  RT_CALLRA(0x242db0, guest::vifUploadColours(0x2a4370,0x2a4360));
  puVar1 = 0x0177aec0;
  iVar3 = (int)(*(int16_t*)rtguest::G(0x177ae24));  // heading halfword table: MIPS 0x242db8 lh 0x1d4($18)
  RT_CALLRA(0x242dcc, guest::vu0UnitMatrix(afStack_170));
  (*(float*)rtguest::G((uint32_t)afStack_170 + (uint32_t)(0) * 4)) = fVar4;
  (*(float*)rtguest::G((uint32_t)fStack_148)) = fVar4;
  RT_CALLRA(0x242dec, guest::vu0RotMatrixY((float)iVar3 * fVar5 * 3.0517578e-05f,afStack_170,afStack_170));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_140)) = (*(uint32_t*)rtguest::G(0x177ace0));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_138)) = (*(uint32_t*)rtguest::G(0x177ace8));
  RT_CALLRA(0x242e04, guest::vifObjectMatrix(afStack_170));
  RT_CALLRA(0x242e14, guest::vifCallModel(0x290870));
  if (((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 10))) & 2) != 0) {
    RT_CALLRA(0x242e34, guest::vifUploadColours(0x2a4380,0x2a4360));
    iVar3 = (int)(*(uint32_t*)rtguest::G(0x177b094));
    uVar2 = 2;
    RT_CALLRA(0x242e50, guest::vu0UnitMatrix(afStack_130));
    puVar1 = 0x0177b130;
    (*(float*)rtguest::G((uint32_t)afStack_130 + (uint32_t)(0) * 4)) = fVar4;
    (*(float*)rtguest::G((uint32_t)fStack_108)) = fVar4;
    RT_CALLRA(0x242e74, guest::vu0RotMatrixY((float)iVar3 * fVar5 * 3.0517578e-05f,afStack_130,afStack_130));
    (*(uint32_t*)rtguest::G((uint32_t)uStack_100)) = (*(uint32_t*)rtguest::G(0x177af50));
    (*(uint32_t*)rtguest::G((uint32_t)uStack_f8)) = (*(uint32_t*)rtguest::G(0x177af58));
    RT_CALLRA(0x242e8c, guest::vifObjectMatrix(afStack_130));
    RT_CALLRA(0x242e94, guest::vifCallModel(0x290870));
  }
  RT_CALLRA(0x242ea4, guest::vifUploadColours(0x2a4390,0x2a4360));
  if (uVar2 < 0x18) {
    do {
      if (((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x198))) != 0) && (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x198))) & 0x30) == 0)) {
        RT_CALLRA(0x242edc, guest::vu0UnitMatrix(afStack_f0));
        (*(uint32_t*)rtguest::G((uint32_t)uStack_c0)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x90)));
        (*(uint32_t*)rtguest::G((uint32_t)uStack_b8)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x98)));
        (*(float*)rtguest::G((uint32_t)afStack_f0 + (uint32_t)(0) * 4)) = fVar4;
        (*(float*)rtguest::G((uint32_t)fStack_c8)) = fVar4;
        RT_CALLRA(0x242efc, guest::vifObjectMatrix(afStack_f0));
        RT_CALLRA(0x242f04, guest::vifCallModel(0x290940));
      }
      uVar2 = uVar2 + 1;
      puVar1 = puVar1 + 0x270;
    } while ((int)uVar2 < 0x18);
  }
  puVar1 = 0x0177aec0;
  iVar3 = 5;
  do {
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x198))) & 0x30) != 0) {
      if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x198))) & 0x40) == 0) {
        RT_CALLRA(0x242f4c, guest::vifUploadColours(0x2a4370,0x2a4360));
      }
      else {
        RT_CALLRA(0x242f64, guest::vifUploadColours(0x2a4380,0x2a4360));
      }
      RT_CALLRA(0x242f70, guest::vu0UnitMatrix(afStack_b0));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_80)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x90)));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_78)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x98)));
      (*(float*)rtguest::G((uint32_t)afStack_b0 + (uint32_t)(0) * 4)) = fVar4;
      (*(float*)rtguest::G((uint32_t)fStack_88)) = fVar4;
      RT_CALLRA(0x242f90, guest::vifObjectMatrix(afStack_b0));
      RT_CALLRA(0x242f98, guest::vifCallModel(0x290940));
    }
    iVar3 = iVar3 + -1;
    puVar1 = puVar1 + 0x270;
  } while (-1 < iVar3);
  return 0;
}


RT_GUEST_FN(0x00242d20, guest::raceMapMarkers)

}  // namespace guest
