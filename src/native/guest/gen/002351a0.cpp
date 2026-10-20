// guest:0x002351a0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetHudState2(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
uint32_t __s;
  int iVar1;
  
  __s = (uint32_t)(param_1 + 0x82b4);
  iVar1 = 0;
  if (param_2 != 0) {
    iVar1 = 3;
  }
  if ((*(int32_t*)rtguest::G((uint32_t)__s)) != iVar1) {
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x2c));
    (*(int32_t*)rtguest::G((uint32_t)__s)) = iVar1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82dc))) = 0x40000000;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82b8))) = 0x140;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82bc))) = 0xe0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82c4))) = 2;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82c8))) = 0x0029faa8;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82cc))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82d8))) = 0x1e;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82c0))) = 1;
  }
  return 0;
}


RT_GUEST_FN(0x002351a0, guest::SetHudState2)

}  // namespace guest
