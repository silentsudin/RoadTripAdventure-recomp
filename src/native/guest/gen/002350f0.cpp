// guest:0x002350f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetHudState(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x28) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x2c) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_5);  // walk: = param_5
    rtguest::setS32(2, (uint32_t)param_4);  // walk: = param_4
    rtguest::setS32(3, (uint32_t)param_3);  // walk: = param_3
  }
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8004)));
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)((uint32_t)(param_1 + 0x8438))), 0, 0x14));
  uVar2 = 0x140;
  if (iVar1 + param_2 != 0) {
    uVar2 = 0x230;
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8444))) = param_3;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8448))) = param_4;
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8438))) = param_5;
  if (param_5 == 1) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x843c))) = uVar2;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8440))) = 0x2c;
  }
  else {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8440))) = 0xe0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x843c))) = 0x140;
  }
  return 0;
}


RT_GUEST_FN(0x002350f0, guest::SetHudState)

}  // namespace guest
