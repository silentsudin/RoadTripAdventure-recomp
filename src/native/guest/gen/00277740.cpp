// guest:0x00277740 translated from Ghidra pseudocode and ELF disassembly
// (= fdlibm a_tanf.c band structure). param_1 carries x's f-register bits;
// Ghidra's puVar5/return fiction (undefined*) is the float t; result returns
// as f-register bits. Tables: 0x331b38 atanhi[], 0x331b48 atanlo[].
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t atanf(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
  int iVar1;
  uint32_t puVar2;
  float fVar3;
  float fVar4;
  float fVar5;  // t (pseudocode puVar5, shown as undefined* — float fiction)
  float fVar6;

  puVar2 = param_1 & 0x7fffffffu;
  if (0x507fffffu < puVar2) {
    if (0 < (int32_t)param_1) {
      return 0x3fc90fdbu;
    }
    return 0xbfc90fdbu;
  }
  if (puVar2 < 0x3ee00000u) {
    fVar5 = rtguest::fbits(param_1);
    iVar1 = -1;
    if (puVar2 <= 0x30ffffffu) {
      if (1.0f < rtguest::fbits(param_1) + 1e+30f) {
        return param_1;
      }
      fVar6 = rtguest::fbits(param_1) * rtguest::fbits(param_1);
      goto LAB_002778f0;
    }
  }
  else {
    fVar6 = rtguest::fbits(RT_CALLRA(0x277800, guest::fabsf(param_1)));
    if (puVar2 < 0x3f980000u) {
      if (puVar2 < 0x3f300000u) {
        fVar5 = rtguest::fdiv(((fVar6 + fVar6) - 1.0f), (fVar6 + 2.0f));
        iVar1 = 0;
      }
      else {
        fVar5 = rtguest::fdiv((fVar6 - 1.0f), (fVar6 + 1.0f));
        iVar1 = 1;
      }
    }
    else if (puVar2 < 0x401c0000u) {
      fVar5 = rtguest::fdiv((fVar6 - 1.5f), (fVar6 * 1.5f + 1.0f));
      iVar1 = 2;
    }
    else {
      iVar1 = 3;
      fVar5 = rtguest::fdiv(-1.0f, fVar6);  // div.s (round-nearest)
    }
  }
  fVar6 = fVar5 * fVar5;
LAB_002778f0:
  fVar4 = fVar6 * fVar6;
  fVar3 = fVar4 * (fVar4 * (fVar4 * (fVar4 * (fVar4 * -0.03653157f + -0.058335703f) + -0.07691876f) +
                           -0.111111104f) + -0.2f);
  fVar6 = fVar6 * (fVar4 * (fVar4 * (fVar4 * (fVar4 * (fVar4 * 0.01628582f + 0.04976878f) + 0.06661073f
                                             ) + 0.09090887f) + 0.14285715f) + 0.33333334f);
  if (iVar1 < 0) {
    fVar5 = fVar5 - fVar5 * (fVar6 + fVar3);
  }
  else {
    fVar5 = (*(float*)rtguest::G((uint32_t)(0x00331b38 + iVar1 * 4))) -
            (((fVar5 * (fVar6 + fVar3) - (*(float*)rtguest::G((uint32_t)(0x00331b48 + iVar1 * 4)))) - fVar5));
    if ((int32_t)param_1 < 0) {
      fVar5 = -fVar5;
    }
  }
  return rtguest::f2bits(fVar5);
}


RT_GUEST_FN(0x00277740, guest::atanf)

}  // namespace guest
