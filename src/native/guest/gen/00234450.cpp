// guest:0x00234450 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetEngineConfig(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
  }
  int iVar1;
  undefined4 uVar2;
uint32_t __s;
  int iVar3;
  
  iVar3 = 0;
  if (param_3 != 0) {
    iVar3 = 3;
  }
  iVar1 = param_1 + param_2 * 0x408;
  __s = (uint32_t)(iVar1 + 0x8640);
  param_2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8004))) + param_2;
  if ((*(int32_t*)rtguest::G((uint32_t)__s)) != iVar3) {
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x2c));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8644))) = 0x140;
    uVar2 = 0x70;
    (*(int32_t*)rtguest::G((uint32_t)__s)) = iVar3;
    if ((param_2 != 0) && (param_2 != 1)) {
      uVar2 = 0x150;
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8648))) = uVar2;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8668))) = 0x40000000;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8650))) = 10;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8654))) = 0x0029f7c8;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8658))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x864c))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8664))) = 0;
  }
  return 0;
}


RT_GUEST_FN(0x00234450, guest::SetEngineConfig)

}  // namespace guest
