// guest:0x00234b98 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetRoadData6(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
  int iVar1;
  undefined4 uVar2;
uint32_t __s;
  int iVar3;
  
  iVar3 = 0;
  iVar1 = param_2 * 0x408 + param_1;
  __s = (uint32_t)(iVar1 + 0x8748);
  if (param_3 != 0) {
    iVar3 = 2;
  }
  param_2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8004))) + param_2;
  if ((*(int32_t*)rtguest::G((uint32_t)__s)) != iVar3) {
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x2c));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x874c))) = 0x140;
    (*(int32_t*)rtguest::G((uint32_t)__s)) = iVar3;
    if (param_2 == 0) {
      uVar2 = 0xa8;
    }
    else {
      uVar2 = 0x70;
      if (param_2 != 1) {
        uVar2 = 0x150;
      }
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8750))) = uVar2;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8770))) = 0x40000000;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8758))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x875c))) = 0x0029fa88;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8760))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x876c))) = 0x1e;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8754))) = 0;
  }
  return 0;
}


RT_GUEST_FN(0x00234b98, guest::SetRoadData6)

}  // namespace guest
