// guest:0x00288170 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifRegisterRpc(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, uint32_t param_7, ...) {
  rtguest::FrameGuard rt_fg(128);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x70) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x74) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_3);  // walk: = param_3
    rtguest::setS32(2, (uint32_t)param_4);  // walk: = param_4
    rtguest::setS32(3, (uint32_t)param_5);  // walk: = param_5
    rtguest::setS32(4, (uint32_t)param_6);  // walk: = param_6
    rtguest::setS32(5, (uint32_t)param_1);  // walk: = param_1
  }
  int iVar1;
  int iVar2;
  
  RT_CALLRA(0x2881ac, guest::DIntr());
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0xf) * sizeof(uint32_t))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0xe) * sizeof(uint32_t))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)param_1)) = param_2;
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_7 + 8)));
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint32_t))) = param_3;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) = param_4;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(uint32_t))) = param_5;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(5) * sizeof(uint32_t))) = param_6;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0x10) * sizeof(uint32_t))) = param_7;
  if (iVar2 == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_7 + 8))) = param_1;
  }
  else {
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x38)));
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x38))) == 0) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x38))) = param_1;
    }
    else {
      do {
        iVar2 = iVar1;
        iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x38)));
      } while (iVar1 != 0);
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x38))) = param_1;
    }
  }
  EIntr();
  return 0;
}


RT_GUEST_FN(0x00288170, guest::sceSifRegisterRpc)

}  // namespace guest
