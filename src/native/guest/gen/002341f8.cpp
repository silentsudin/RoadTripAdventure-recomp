// guest:0x002341f8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetTailConfig(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  int iVar1;
  undefined4 uVar2;
uint32_t __s;
  
  iVar1 = param_1 + param_2 * 0x408;
  __s = (uint32_t)(iVar1 + 0x85bc);
  param_2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8004))) + param_2;
  if ((*(uint32_t*)rtguest::G((uint32_t)__s)) != (uint32_t)(param_3 != 0)) {
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x2c));
    uVar2 = 0x70;
    (*(uint32_t*)rtguest::G((uint32_t)__s)) = (uint32_t)(param_3 != 0);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x85c0))) = 0x140;
    if ((param_2 != 0) && (param_2 != 1)) {
      uVar2 = 0x150;
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x85c4))) = uVar2;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x85e4))) = 0x3f800000;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x85d0))) = 0x0029f708;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x85d4))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x85e0))) = 0x1e;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x85c8))) = 1;
  }
  return 0;
}


RT_GUEST_FN(0x002341f8, guest::SetTailConfig)

}  // namespace guest
