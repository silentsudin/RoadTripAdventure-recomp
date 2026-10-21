// guest:0x00254290 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RotateObjectByWrappedYaw(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(112);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x58, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x50, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x68) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x6c) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
    rtguest::gS[2] = rt_sv + 0x40ull;  // walk: sp+0x40
  }
uint32_t auStack_70 = rtguest::allocFrame(64);
uint32_t afStack_30 = rtguest::allocFrame(8);
    uint32_t uStack_28 = rtguest::allocFrame(4);
  int iVar1;
  
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(afStack_30)), 0, 0x10));
  iVar1 = (int)param_1;
  for ((*(float*)rtguest::G((uint32_t)uStack_28)) = -(*(float*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x30))); (*(float*)rtguest::G((uint32_t)uStack_28)) < (*(float*)rtguest::G(0x333078));
      (*(float*)rtguest::G((uint32_t)uStack_28)) = (*(float*)rtguest::G((uint32_t)uStack_28)) + (*(float*)rtguest::G(0x33307c))) {
  }
  if ((*(float*)rtguest::G(0x333080)) < (*(float*)rtguest::G((uint32_t)uStack_28))) {
    do {
      (*(float*)rtguest::G((uint32_t)uStack_28)) = (*(float*)rtguest::G((uint32_t)uStack_28)) - (*(float*)rtguest::G(0x333084));
    } while ((*(float*)rtguest::G(0x333080)) < (*(float*)rtguest::G((uint32_t)uStack_28)));
    (*(float*)rtguest::G((uint32_t)afStack_30 + (uint32_t)(0) * 4)) = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x38)));
  }
  else {
    (*(float*)rtguest::G((uint32_t)afStack_30 + (uint32_t)(0) * 4)) = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x38)));
  }
  for (; (*(float*)rtguest::G((uint32_t)afStack_30 + (uint32_t)(0) * 4)) < (*(float*)rtguest::G(0x333088)); (*(float*)rtguest::G((uint32_t)afStack_30 + (uint32_t)(0) * 4)) = (*(float*)rtguest::G((uint32_t)afStack_30 + (uint32_t)(0) * 4)) + (*(float*)rtguest::G(0x33308c))) {
  }
  for (; (*(float*)rtguest::G(0x333090)) < (*(float*)rtguest::G((uint32_t)afStack_30 + (uint32_t)(0) * 4)); (*(float*)rtguest::G((uint32_t)afStack_30 + (uint32_t)(0) * 4)) = (*(float*)rtguest::G((uint32_t)afStack_30 + (uint32_t)(0) * 4)) - (*(float*)rtguest::G(0x333094))) {
  }
  iVar1 = iVar1 + 0x10;
  RT_CALLRA(0x2543c4, guest::vu0UnitMatrix(auStack_70));
  RT_CALLRA(0x2543d4, guest::vu0RotMatrix(auStack_70,auStack_70,afStack_30));
  RT_CALLRA(0x2543e4, guest::vu0ApplyMatrix(iVar1,auStack_70,iVar1));
  RT_CALLRA(0x2543f4, guest::vu0ApplyMatrix(param_1,auStack_70,param_1));
  RT_CALLRA(0x254400, guest::vecNormalizeVu0(iVar1,iVar1));
  RT_CALLRA(0x25440c, guest::vecNormalizeVu0(param_1,param_1));
  return 0;
}


RT_GUEST_FN(0x00254290, guest::RotateObjectByWrappedYaw)

}  // namespace guest
